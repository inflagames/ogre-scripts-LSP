//
// Created by gonzalezext on 08.02.24.
//

#include "../inc/formatter.h"

// toDo (gonzalezext)[07.02.24]: support for trimFinalNewLines is not implemented
ResultArray *OgreScriptLSP::Formatter::formatting(Parser *parser, FormattingOptions options, Range range) {
    auto *res = new ResultArray();

    // initial values
    parser->initSwap();
    int level = 0;

    // line
    int previousTokenPosition = 0;
    bool firstInLine = true;
    TokenValue lastToken;
    while (!parser->isEof()) {
        auto tk = parser->getToken();

        lastToken = tk;

        if (tk.tk == endl_tk && !options.trimTrailingWhitespace) {
            // remove trailing whitespaces
            previousTokenPosition = 0;
            firstInLine = true;
            parser->nextToken();
            continue;
        }

        if (tk.tk == right_curly_bracket_tk) {
            level--;
        }

        int position;

        if (!firstInLine) {
            // toDo (gonzalezext)[28.01.24]: calculate separation base on the tokens
            if (tk.tk == endl_tk) {
                position = previousTokenPosition;
            } else {
                position = previousTokenPosition + 1;
            }
        }

        if (firstInLine) {
            if (tk.tk != endl_tk) {
                std::string nexText;
                if (options.insertSpaces) {
                    nexText = repeatCharacter(' ', level * (int) options.tabSize);
                } else {
                    nexText = repeatCharacter('\t', level);
                }
                res->elements.push_back(new TextEdit({tk.line, previousTokenPosition},
                                                     {tk.line, tk.column},
                                                     nexText));
            }
        } else {
            if (tk.column > position) {
                res->elements.push_back(new TextEdit({tk.line, position},
                                                     {tk.line, tk.column},
                                                     ""));
            } else if (tk.column < position) {
                std::string nexText = repeatCharacter(' ', position - tk.column);
                res->elements.push_back(new TextEdit({tk.line, previousTokenPosition},
                                                     {tk.line, previousTokenPosition},
                                                     nexText));
            }
        }

        parser->nextToken();
        if (tk.tk != endl_tk) {
            previousTokenPosition = tk.column + tk.size;
            firstInLine = false;
        } else {
            previousTokenPosition = 0;
            firstInLine = true;
        }

        if (tk.tk == left_curly_bracket_tk) {
            level++;
        }
    }

    // add end line
    if (options.insertFinalNewline && lastToken.tk != endl_tk) {
        res->elements.push_back(new TextEdit({lastToken.line, lastToken.column + lastToken.size},
                                             {lastToken.line, lastToken.column + lastToken.size},
                                             "\n"));
    }

    // mainly for range formatting
    for (auto it = res->elements.begin(); it != res->elements.end(); it++) {
        auto e = (TextEdit *) *it;
        if (!range.inRange(e->range)) {
            res->elements.erase(it);
        }
    }

    return res;
}

std::string OgreScriptLSP::Formatter::repeatCharacter(char c, int times) {
    std::string text;
    for (int i = 0; i < times; i++) {
        text.push_back(c);
    }
    return text;
}
