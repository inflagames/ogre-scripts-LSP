#include "../inc/scanner.h"

OgreScriptLSP::Scanner::Scanner() = default;

void OgreScriptLSP::Scanner::loadScript(const std::string &scriptFile, const std::string &code) {
    codeStream.reset();
    if (code.empty()) {
        auto file = std::make_unique<std::ifstream>();

        file->open(scriptFile);
        if (!file->is_open()) {
            throw FileException(FILE_IS_NOT_OPEN_OR_NOT_EXIST);
        }
        codeStream = std::move(file);
    } else {
        // open code from string
        auto codeStr = std::make_unique<std::stringstream>(code);
        codeStream = std::move(codeStr);
    }
    ch = ' ';
    isNewLine = false;
    lineCount = 0;
    columnCount = -1;
}

std::vector<OgreScriptLSP::TokenValue> OgreScriptLSP::Scanner::parse() {
    exceptions.clear();
    comments.clear();
    std::vector<OgreScriptLSP::TokenValue> list;
    while (true) {
        auto tk = nextToken();
        if (tk.tk == EOF_tk) {
            break;
        }
        list.push_back(tk);
    }

    codeStream.reset();

    return list;
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::nextToken() {
    std::string tkStr;

    while (!codeStream->eof() && consumeEmpty()) {
        switch (ch) {
            case '\n':
                return symbolToken(endl_tk);
            case '/': {
                nextCharacter();
                if (ch == '/') {
                    // consume comment
                    consumeComment();
                    continue;
                }
                recuperateError(columnCount - 1, SCANNER_INVALID_CHARACTER);
                continue;
            }
            case ':':
                return symbolToken(colon_tk);
            case '{':
                return symbolToken(left_curly_bracket_tk);
            case '}':
                return symbolToken(right_curly_bracket_tk);
            case '"':
                try {
                    return consumeString('"');
                } catch (const ScannerException &e) {
                    exceptions.push_back(e);
                    continue;
                }
            case '*': {
                auto tk = symbolToken(asterisk_tk);
                if (isInvalidMatchDigit(ch)) {
                    return tk;
                }
                try {
                    return consumeMatch();
                } catch (ScannerException &e) {
                    recuperateError(e.range.start.character, e.message);
                    continue;
                }
            }
            case '-':
            case '.': {
                std::string numberPrefix;
                if (ch == '-') {
                    numberPrefix.push_back(ch);
                    nextCharacter();
                    if (!isdigit(ch) && ch != '.') {
                        recuperateError(columnCount - 1, SCANNER_INVALID_CHARACTER);
                        continue;
                    }
                }
                bool isFirstPeriod = true;
                if (ch == '.') {
                    numberPrefix.push_back(ch);
                    nextCharacter();
                    if (!isdigit(ch)) {
                        recuperateError(columnCount - 1, SCANNER_INVALID_CHARACTER);
                        continue;
                    }
                    isFirstPeriod = false;
                }
                try {
                    return consumeNumber(numberPrefix, isFirstPeriod);
                } catch (const ScannerException &e) {
                    recuperateError(e.range.start.character, e.message);
                    continue;
                }
            }
            default:
                if (isdigit(ch)) {
                    try {
                        return consumeNumber();
                    } catch (const ScannerException &e) {
                        recuperateError(e.range.start.character, e.message);
                        continue;
                    }
                }
                // read literals
                if (validLiteral(ch)) {
                    try {
                        auto tk = nextLiteral();
                        return tk;
                    } catch (const ScannerException &e) {
                        exceptions.push_back(e);
                        continue;
                    }
                }
                recuperateError(columnCount, SCANNER_INVALID_CHARACTER);
                continue;
        }
    }

    return {EOF_tk};
}

void OgreScriptLSP::Scanner::consumeComment(bool lineComment) {
    int line = lineCount;
    int column = columnCount - 1;
    nextCharacter(); // consume second /
    do {
        if (lineComment && ch == '\n') {
            comments.push_back({(uint32_t) line, (uint32_t) column, (uint32_t) columnCount - column, 1, 0});
            return;
        }
    } while (nextCharacter());
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::symbolToken(OgreScriptLSP::Token tk) {
    TokenValue res = {tk, ".", lineCount, columnCount};
    nextCharacter();
    return res;
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeNumber(std::string prefix, bool isFirstPeriod) {
    std::string literal = std::move(prefix);
    int line = lineCount, startPos = (int) literal.size();
    int column = columnCount - startPos, rangeLength = startPos;
    bool isFirstCharacterIndicator = true;
    while (!codeStream->eof()) {
        literal.push_back(ch);
        if (nextCharacter() && isFirstCharacterIndicator && (isdigit(ch) || (ch == '.' && isFirstPeriod))) {
            if (ch == '.') {
                isFirstPeriod = false;
            }
        } else if (isFirstCharacterIndicator && validNumberTypeIndicator(ch)) {
            isFirstCharacterIndicator = false;
        } else if (ch == '\n' || ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r' || ch == ',') {
            return {number_literal, literal, line, column, (int) literal.size()};
        } else if (validLiteral(ch, false)) {
            return nextLiteral(literal);
        } else {
            throw ScannerException(SCANNER_INVALID_NUMBER, Range::toRange(line, column, rangeLength));
        }
    }
    throw ScannerException(SCANNER_EOF_ERROR, Range::toRange(line, column, rangeLength));
}

bool OgreScriptLSP::Scanner::validNumberTypeIndicator(char ch) {
    return ch == 'f';
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeString(char stringDelimiter) {
    std::string literal;
    int line = lineCount;
    int column = columnCount, rangeLength = 1;
    nextCharacter();
    while (!codeStream->eof()) {
        rangeLength++;
        if (ch == '\n') {
            throw ScannerException(SCANNER_INVALID_STRING_LITERAL, Range::toRange(line, column, rangeLength - 1));
        } else if (ch == stringDelimiter) {
            nextCharacter();
            return {string_literal, literal, line, column, (int) literal.size() + 2};
        } else if (ch == '\\') {
            nextCharacter();
        }
        literal.push_back(ch);
        nextCharacter();
    }
    throw ScannerException(SCANNER_EOF_ERROR, Range::toRange(line, column, rangeLength));
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeMatch() {
    std::string literal = "*";
    int line = lineCount;
    int column = columnCount - 1, rangeLength = 0;
    while (!codeStream->eof()) {
        rangeLength++;
        if (isInvalidMatchDigit(ch)) {
            throw ScannerException(SCANNER_INVALID_MATCH_LITERAL, Range::toRange(line, column, rangeLength));
        } else if (ch == '*') {
            literal.push_back(ch);
            nextCharacter();
            return {match_literal, literal, line, column, (int) literal.size()};
        } else if (ch == '\\') {
            nextCharacter();
        }
        literal.push_back(ch);
        nextCharacter();
    }
    throw ScannerException(SCANNER_EOF_ERROR, Range::toRange(line, column, rangeLength));
}

bool OgreScriptLSP::Scanner::isInvalidMatchDigit(char c) {
    return c == '\n' || c == '\t' || c == ' ' || c == '\v' || c == '\r' || c == '\f';
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::nextLiteral(std::string prefix) {
    std::string literal = prefix;
    int line = lineCount;
    int column = columnCount - (int) prefix.size();
    while (!codeStream->eof()) {
        literal.push_back(ch);
        if (!nextCharacter() || !validLiteral(ch, false)) {
            if (literal == "abstract") {
                return {abstract_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "import") {
                return {import_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "from") {
                return {from_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "default_params") {
                return {default_params_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "fragment_program") {
                return {fragment_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "fragment_program_ref") {
                return {fragment_program_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "material") {
                return {material_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "pass") {
                return {pass_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "rtshader_system") {
                return {rtshader_system_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "technique") {
                return {technique_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "texture_unit") {
                return {texture_unit_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "vertex_program") {
                return {vertex_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "vertex_program_ref") {
                return {vertex_program_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "shared_params") {
                return {shared_params_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "shared_params_ref") {
                return {shared_params_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "shadow_caster_material") {
                return {shadow_caster_material_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "shadow_receiver_material") {
                return {shadow_receiver_material_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "geometry_program") {
                return {geometry_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "geometry_program_ref") {
                return {geometry_program_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "tessellation_hull_program") {
                return {tessellation_hull_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "tessellation_hull_program_ref") {
                return {tessellation_hull_program_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "tessellation_domain_program") {
                return {tessellation_domain_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "tessellation_domain_program_ref") {
                return {tessellation_domain_program_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "compute_program") {
                return {compute_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "compute_program_ref") {
                return {compute_program_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "texture_source") {
                return {texture_source_tk, literal, line, column, (int) literal.size()};
            } else if (literal.starts_with('$')) {
                return {variable, literal, line, column, (int) literal.size()};
            }
            return {identifier, literal, line, column, (int) literal.size()};
        }
    }
    throw ScannerException(SCANNER_EOF_ERROR, Range::toRange(line, column, (int) literal.size()));
}

bool OgreScriptLSP::Scanner::validLiteral(char c, bool startCharacter) {
    if (startCharacter) {
        return isalnum(c) || c == '_' || c == '$' || c == '/' || c == '.';
    }
    return isalnum(c) || c == '_' || c == '/' || c == '.' || c == '&';
}

void OgreScriptLSP::Scanner::recuperateError(int column, const std::string &error) {
    int rangeLength = columnCount - column;
    while (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\f' && ch != '\v' && ch != '\r') {
        rangeLength++;
        if (!nextCharacter()) {
            break;
        }
    }
    if (!error.empty()) {
        exceptions.push_back(ScannerException(error, Range::toRange(lineCount, column, rangeLength)));
    }
    consumeEmpty();
}

bool OgreScriptLSP::Scanner::consumeEmpty() {
    while (ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r') {
        if (!nextCharacter()) {
            return false;
        }
    }
    return true;
}

bool OgreScriptLSP::Scanner::nextCharacter() {
    codeStream->get(ch);
    if (isNewLine) {
        lineCount++;
        columnCount = 0;
    } else {
        columnCount++;
    }
    isNewLine = ch == '\n';
    return !codeStream->eof();
}
