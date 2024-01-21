#include "../inc/scanner.h"

#include <utility>

Jsai::Scanner::Scanner() = default;

void Jsai::Scanner::loadScript(const std::string &scriptFile) {
    file.open(scriptFile);
    ch = ' ';
}

std::vector<Jsai::TokenValue> Jsai::Scanner::parse() {
    std::vector<Jsai::TokenValue> list;
    while (true) {
        auto tk = nextToken();
        if (tk.tk == EOF_tk) {
            break;
        }
        list.push_back(tk);
    }
    return list;
}

Jsai::TokenValue Jsai::Scanner::nextToken() {
    std::string tkStr;

    while (consumeEmpty()) {
        switch (ch) {
            case ':':
                return symbolToken(colon_tk);
            case ';':
                return symbolToken(semicolon_tk);
            case ',':
                return symbolToken(comma_tk);
            case '\\':
                return symbolToken(backslash_tk);
            case '(':
                return symbolToken(left_parenthesis_tk);
            case ')':
                return symbolToken(right_parenthesis_tk);
            case '{':
                return symbolToken(left_curly_bracket_tk);
            case '[':
                return symbolToken(left_square_bracket_tk);
            case ']':
                return symbolToken(right_square_bracket_tk);
            case '"': {
                symbolToken(double_quote_tk);
                return consumeString('"');
            }
            case '\'':
                symbolToken(single_quote_tk);
                return consumeString('\'');
            case '`':
                // toDo (gonzalezext)[21.01.24]: tbd probably not supported (to be considered)
                return symbolToken(back_apostrophe);
            case '=':
                return symbolToken(equals_tk);
            case '+':
                return symbolToken(plus_tk);
            case '-':
                return symbolToken(minus_tk);
            case '*':
                return symbolToken(asterisk_tk);
            case '/': {
                auto tkValue = symbolToken(slash_tk);
                // check if is a comment to consume it
                if (ch == '/') {
                    consumeComment();
                    continue;
                }
                if (ch == '*') {
                    consumeComment(false);
                    continue;
                }
                return tkValue;
            }
            case '~':
                return symbolToken(tilde_accent_tk);
            case '!':
                return symbolToken(exclamation_tk);
            case '<':
                return symbolToken(lt_tk);
            case '>':
                return symbolToken(gt_tk);
            case '&':
                return symbolToken(ampersand_tk);
            case '|':
                return symbolToken(vertical_bar_tk);
            case '.': {
                auto tkValue = symbolToken(period_tk);
                // handle number literal that start with period
                if (isdigit(ch)) {
                    tkValue = consumeNumber(false);
                    tkValue.literal.insert(0, 1, '.');
                }
                return tkValue;
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

void Jsai::Scanner::consumeComment(bool lineComment) {
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

Jsai::TokenValue Jsai::Scanner::symbolToken(Jsai::Token tk) {
    nextCharacter();
    return {tk};
}

Jsai::TokenValue Jsai::Scanner::consumeNumber(bool isFirstPeriod) {
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

Jsai::TokenValue Jsai::Scanner::consumeString(char stringDelimiter) {
    std::string literal;
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

Jsai::TokenValue Jsai::Scanner::nextLiteral() {
    std::string literal;
    while (true) {
        literal.push_back(ch);
        if (!nextCharacter() || !validLiteral(ch, false)) {
            if (literal == "function") {
                return {function_tk};
            } else if (literal == "request") {
                return {request_tk};
            } else if (literal == "let") {
                return {let_tk};
            } else if (literal == "const") {
                return {const_tk};
            } else if (literal == "var") {
                return {var_tk};
            } else if (literal == "if") {
                return {if_tk};
            } else if (literal == "else") {
                return {else_tk};
            } else if (literal == "for") {
                return {for_tk};
            } else if (literal == "do") {
                return {do_while_tk};
            } else if (literal == "while") {
                return {while_tk};
            } else if (literal == "true") {
                return {true_tk};
            } else if (literal == "false") {
                return {false_tk};
            }
            return {identifier, literal};
        }
    }
}

bool Jsai::Scanner::validLiteral(char c, bool startCharacter) {
    if (startCharacter) {
        return isalpha(c) || c == '_' || c == '$';
    }
    return isalnum(c) || c == '_' || c == '$';
}

bool Jsai::Scanner::consumeEmpty() {
    while (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r') {
        if (!nextCharacter()) {
            return false;
        }
    }
    return true;
}

bool Jsai::Scanner::nextCharacter() {
    file.get(ch);
    return !file.eof();
}
