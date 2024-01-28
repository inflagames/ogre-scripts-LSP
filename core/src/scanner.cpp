#include "../inc/scanner.h"

OgreScriptLSP::Scanner::Scanner() = default;

void OgreScriptLSP::Scanner::loadScript(const std::string &scriptFile) {
    file.open(scriptFile);
    if (!file.is_open()) {
        throw FileException(FILE_IS_NOT_OPEN_OR_NOT_EXIST);
    }
    ch = ' ';
    lineCount = 1;
    columnCount = 0;
}

std::vector<OgreScriptLSP::TokenValue> OgreScriptLSP::Scanner::parse() {
    if (!file.is_open()) {
        throw FileException(FILE_IS_NOT_OPEN_OR_NOT_EXIST);
    }

    std::vector<OgreScriptLSP::TokenValue> list;
    while (true) {
        auto tk = nextToken();
        if (tk.tk == EOF_tk) {
            break;
        }
        list.push_back(tk);
    }
    return list;
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::nextToken() {
    std::string tkStr;

    while (!file.eof() && consumeEmpty()) {
        switch (ch) {
            case '\n':
                return symbolToken(endl_tk);
            case '/': {
                nextCharacter();
                // check if is a comment to consume it
                if (ch == '/') {
                    consumeComment();
                    continue;
                }
                // toDo (gonzalezext)[25.01.24]: check if multiline comments are permitted
//                if (ch == '*') {
//                    consumeComment(false);
//                    continue;
//                }
                // todo: exception if not comment
            }
            case ':':
                return symbolToken(colon_tk);
            case ';':
                return symbolToken(semicolon_tk);
                // toDo (gonzalezext)[25.01.24]: not sure if parenthesis are used
//            case '(':
//                return symbolToken(left_parenthesis_tk);
//            case ')':
//                return symbolToken(right_parenthesis_tk);
            case '{':
                return symbolToken(left_curly_bracket_tk);
            case '}':
                return symbolToken(right_curly_bracket_tk);
            case '"':
                return consumeString('"');
            case '*': {
                // toDo (gonzalezext)[25.01.24]: validate if the * as empty character after in that case return asterisk_tk
                auto res = consumeString('*');
                return {match_literal, res.literal};
            }
                // toDo (gonzalezext)[25.01.24]: check if string literals can be defined with simple_quote_tk
//            case '\'':
//                return consumeString('\'');
            case '-':
            case '.': {
                // toDo (gonzalezext)[25.01.24]: handle number that start with minus
                // handle number literal that start with period
                if (isdigit(ch)) {
                    auto tkValue = consumeNumber(false);
                    tkValue.literal.insert(0, 1, '.');
                } else {
                    // toDo (gonzalezext)[25.01.24]: throw error here
                }
                return {EOF_tk};
            }
            default:
                if (isdigit(ch)) {
                    return consumeNumber();
                }
                // read literals
                if (validLiteral(ch)) {
                    return nextLiteral();
                }
                // toDo (gonzalezext)[21.01.24]: throw error here
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
    int column = columnCount;
    while (true) {
        literal.push_back(ch);
        if (nextCharacter() && (isdigit(ch) || (ch == '.' && isFirstPeriod))) {
            if (ch == '.') {
                isFirstPeriod = false;
            }
        } else {
            return {number_literal, literal, line, column, (int) literal.size()};
        }
    }
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeString(char stringDelimiter) {
    std::string literal;
    nextCharacter();
    int line = lineCount;
    int column = columnCount;
    while (true) {
        if (ch == stringDelimiter) {
            nextCharacter();
            return {string_literal, literal, line, column, (int) literal.size() + 2};
        } else if (ch == '\\') {
            nextCharacter();
        }
        literal.push_back(ch);
        nextCharacter();
    }
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::nextLiteral() {
    std::string literal;
    int line = lineCount;
    int column = columnCount;
    while (true) {
        literal.push_back(ch);
        if (!nextCharacter() || !validLiteral(ch, false)) {
            if (literal == "abstract") {
                return {abstract_tk, "abstract", line, column, (int) literal.size()};
            } else if (literal == "default_params") {
                return {default_params_tk, "default_params", line, column, (int) literal.size()};
            } else if (literal == "delegate") {
                return {delegate_tk, "delegate", line, column, (int) literal.size()};
            } else if (literal == "entry_point") {
                return {entry_point_tk, "entry_point", line, column, (int) literal.size()};
            } else if (literal == "fragment_program") {
                return {fragment_program_tk, "fragment_program", line, column, (int) literal.size()};
            } else if (literal == "fragment_program_ref") {
                return {fragment_program_ref_tk, "fragment_program_ref", line, column, (int) literal.size()};
            } else if (literal == "material") {
                return {material_tk, "material", line, column, (int) literal.size()};
            } else if (literal == "pass") {
                return {pass_tk, "pass", line, column, (int) literal.size()};
            } else if (literal == "profiles") {
                return {profiles_tk, "profiles", line, column, (int) literal.size()};
            } else if (literal == "technique") {
                return {technique_tk, "technique", line, column, (int) literal.size()};
            } else if (literal == "texture_unit") {
                return {texture_unit_tk, "texture_unit", line, column, (int) literal.size()};
            } else if (literal == "vertex_program") {
                return {vertex_program_tk, "vertex_program", line, column, (int) literal.size()};
            } else if (literal == "vertex_program_ref") {
                return {vertex_program_ref_tk, "vertex_program_ref", line, column, (int) literal.size()};
            } else if (literal.starts_with('$')) {
                return {variable, literal, line, column, (int) literal.size()};
            }
            return {identifier, literal, line, column, (int) literal.size()};
        }
    }
}

bool OgreScriptLSP::Scanner::validLiteral(char c, bool startCharacter) {
    if (startCharacter) {
        return isalpha(c) || c == '_' || c == '$' || c == '/' || c == '.';
    }
    return isalnum(c) || c == '_' || c == '$' || c == '/' || c == '.';
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
    file.get(ch);
    if (ch == '\n') {
        lineCount++;
        columnCount = 0;
    } else {
        columnCount++;
    }
    return !file.eof();
}
