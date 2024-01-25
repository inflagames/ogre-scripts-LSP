#include "../inc/scanner.h"

#include <utility>

OgreScriptLSP::Scanner::Scanner() = default;

void OgreScriptLSP::Scanner::loadScript(const std::string &scriptFile) {
    file.open(scriptFile);
    ch = ' ';
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
    return list;
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::nextToken() {
    std::string tkStr;

    while (consumeEmpty()) {
        switch (ch) {
            case '/': {
                auto tkValue = symbolToken(slash_tk);
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
                return symbolToken(right_parenthesis_tk);
            case '{':
                return symbolToken(left_curly_bracket_tk);
            case '}':
                return symbolToken(right_curly_bracket_tk);
                // toDo (gonzalezext)[25.01.24]: not sure if square bracket are used
//            case '[':
//                return symbolToken(left_square_bracket_tk);
//            case ']':
//                return symbolToken(right_square_bracket_tk);
            case '"':
                return consumeString('"');
            case '*': {
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
    while (nextCharacter()) {
        if ((lineComment && ch == '\n') || (!lineComment && pCh == '/' && ch == '/')) {
            return;
        }
        pCh = ch;
    }
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::symbolToken(OgreScriptLSP::Token tk) {
    nextCharacter();
    return {tk};
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeNumber(bool isFirstPeriod) {
    std::string literal;
    while (true) {
        literal.push_back(ch);
        if (nextCharacter() && (isdigit(ch) || (ch == '.' && isFirstPeriod))) {
            if (ch == '.') {
                isFirstPeriod = false;
            }
        } else {
            return {number_literal, literal};
        }
    }
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::consumeString(char stringDelimiter) {
    std::string literal;
    nextCharacter();
    while (true) {
        if (ch == stringDelimiter) {
            nextCharacter();
            return {string_literal, literal};
        } else if (ch == '\\') {
            nextCharacter();
        }
        literal.push_back(ch);
        nextCharacter();
    }
}

OgreScriptLSP::TokenValue OgreScriptLSP::Scanner::nextLiteral() {
    std::string literal;
    while (true) {
        literal.push_back(ch);
        if (!nextCharacter() || !validLiteral(ch, false)) {
            if (literal == "abstract") {
                return {abstract_tk};
            } else if (literal == "default_params") {
                return {default_params_tk};
            } else if (literal == "delegate") {
                return {delegate_tk};
            } else if (literal == "entry_point") {
                return {entry_point_tk};
            } else if (literal == "fragment_program") {
                return {fragment_program_tk};
            } else if (literal == "material") {
                return {material_tk};
            } else if (literal == "param_named") {
                return {param_named_tk};
            } else if (literal == "pass") {
                return {pass_tk};
            } else if (literal == "profiles") {
                return {profiles_tk};
            } else if (literal == "set") {
                return {set_tk};
            } else if (literal == "source") {
                return {source_tk};
            } else if (literal == "technique") {
                return {technique_tk};
            } else if (literal == "unified") {
                return {unified_tk};
            } else if (literal == "vertex_program") {
                return {vertex_program_tk};
            } else if (literal.starts_with('$')) {
                return {variable, literal};
            }
            return {identifier, literal};
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
    while (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r') {
        if (!nextCharacter()) {
            return false;
        }
    }
    return true;
}

bool OgreScriptLSP::Scanner::nextCharacter() {
    file.get(ch);
    return !file.eof();
}
