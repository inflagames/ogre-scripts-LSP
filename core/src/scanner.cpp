#include "../inc/scanner.h"

OgreScriptLSP::Scanner::Scanner() = default;

void OgreScriptLSP::Scanner::loadScript(const std::string &scriptFile, const std::string &code) {
    delete codeStream;
    if (code.empty()) {
        auto *file = new std::ifstream();

        // close if needed
        if (file->is_open()) {
            file->close();
        }
        file->open(scriptFile);
        if (!file->is_open()) {
            throw FileException(FILE_IS_NOT_OPEN_OR_NOT_EXIST);
        }
        codeStream = static_cast<std::istream *>(file);
    } else {
        // open code from string
        auto *codeStr = new std::stringstream(code);
        codeStream = static_cast<std::istream *>(codeStr);
    }
    ch = ' ';
    isNewLine = false;
    lineCount = 0;
    columnCount = -1;
}

std::vector<OgreScriptLSP::TokenValue> OgreScriptLSP::Scanner::parse() {
    std::vector<OgreScriptLSP::TokenValue> list;
    while (true) {
        auto tk = nextToken();
        if (tk.tk == EOF_tk) {
            break;
        }
        list.push_back(tk);
    }

    delete codeStream;
    codeStream = nullptr;

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
                recuperateError(SCANNER_INVALID_CHARACTER);
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
                auto res = consumeMatch();
                return {match_literal, res.literal};
            }
            case '-':
            case '.': {
                if (ch == '-') {
                    nextCharacter();
                    if (!isdigit(ch)) {
                        recuperateError(SCANNER_INVALID_CHARACTER);
                        continue;
                    }
                }
                bool isFirstPeriod = true;
                if (ch == '.') {
                    nextCharacter();
                    if (!isdigit(ch)) {
                        recuperateError(SCANNER_INVALID_CHARACTER);
                        continue;
                    }
                    isFirstPeriod = false;
                }
                try {
                    auto tkValue = consumeNumber(isFirstPeriod);
                    tkValue.literal.insert(0, 1, isFirstPeriod ? '.' : '-');
                    return tkValue;
                } catch (const ScannerException &e) {
                    exceptions.push_back(e);
                    recuperateError();
                    continue;
                }
            }
            default:
                if (isdigit(ch)) {
                    try {
                        return consumeNumber();
                    } catch (const ScannerException &e) {
                        exceptions.push_back(e);
                        recuperateError();
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
                recuperateError(SCANNER_INVALID_CHARACTER);
                continue;
        }
    }

    return {EOF_tk};
}

void OgreScriptLSP::Scanner::consumeComment(bool lineComment) {
    nextCharacter(); // consume second ch in comment
    nextCharacter(); // consume first ch after comment definition
    char pCh = ch;
    do {
        if ((lineComment && ch == '\n') || (!lineComment && pCh == '*' && ch == '/')) {
            return;
        }
        pCh = ch;
    } while (nextCharacter());
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::symbolToken(OgreScriptLSP::Token tk) {
    TokenValue res = {tk, ".", lineCount, columnCount};
    nextCharacter();
    return res;
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeNumber(bool isFirstPeriod) {
    std::string literal;
    int line = lineCount;
    int column = columnCount, rangeLength = 0;
    while (!codeStream->eof()) {
        literal.push_back(ch);
        if (nextCharacter() && (isdigit(ch) || (ch == '.' && isFirstPeriod))) {
            if (ch == '.') {
                isFirstPeriod = false;
            }
        } else if (ch == '\n' || ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r') {
            return {number_literal, literal, line, column, (int) literal.size()};
        } else {
            throw ScannerException(SCANNER_INVALID_NUMBER, Range::toRange(line, column, rangeLength));
        }
    }
    throw ScannerException(SCANNER_EOF_ERROR, Range::toRange(line, column, rangeLength));
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeString(char stringDelimiter) {
    std::string literal;
    nextCharacter();
    int line = lineCount;
    int column = columnCount, rangeLength = 0;
    while (!codeStream->eof()) {
        rangeLength++;
        if (ch == '\n') {
            throw ScannerException(SCANNER_INVALID_STRING_LITERAL, Range::toRange(line, column, rangeLength));
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
    nextCharacter();
    int line = lineCount;
    int column = columnCount, rangeLength = 0;
    while (!codeStream->eof()) {
        rangeLength++;
        if (ch == '\n' || ch == '\t' || ch == ' ' || ch == '\v' || ch == '\r' || ch == '\f') {
            throw ScannerException(SCANNER_INVALID_MATCH_LITERAL, Range::toRange(line, column, rangeLength));
        } else if (ch == '*') {
            literal.push_back(ch);
            nextCharacter();
            return {string_literal, literal, line, column, (int) literal.size()};
        } else if (ch == '\\') {
            nextCharacter();
        }
        literal.push_back(ch);
        nextCharacter();
    }
    throw ScannerException(SCANNER_EOF_ERROR, Range::toRange(line, column, rangeLength));
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::nextLiteral() {
    std::string literal;
    int line = lineCount;
    int column = columnCount;
    while (!codeStream->eof()) {
        literal.push_back(ch);
        if (!nextCharacter() || !validLiteral(ch, false)) {
            if (literal == "abstract") {
                return {abstract_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "import") {
                return {import_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "default_params") {
                return {default_params_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "entry_point") {
                return {entry_point_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "fragment_program") {
                return {fragment_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "fragment_program_ref") {
                return {fragment_program_ref_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "material") {
                return {material_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "pass") {
                return {pass_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "profiles") {
                return {profiles_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "technique") {
                return {technique_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "texture_unit") {
                return {texture_unit_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "vertex_program") {
                return {vertex_program_tk, literal, line, column, (int) literal.size()};
            } else if (literal == "vertex_program_ref") {
                return {vertex_program_ref_tk, literal, line, column, (int) literal.size()};
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
        return isalpha(c) || c == '_' || c == '$' || c == '/' || c == '.';
    }
    return isalnum(c) || c == '_' || c == '/' || c == '.';
}

void OgreScriptLSP::Scanner::recuperateError(const std::string &error) {
    int rangeLength = 0;
    while (ch != ' ' || ch != '\t' || ch != '\n' || ch != '\f' || ch != '\v' || ch != '\r') {
        rangeLength++;
        if (!nextCharacter()) {
            break;
        }
    }
    if (!error.empty()) {
        exceptions.push_back(ScannerException(error, Range::toRange(lineCount, columnCount, rangeLength)));
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
