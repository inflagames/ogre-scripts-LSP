#include "../inc/formatter.h"

// toDo (gonzalezext)[07.02.24]: support for trimFinalNewLines is not implemented
OgreScriptLSP::ResultArray *
OgreScriptLSP::Formatter::formatting(Parser *parser, FormattingOptions options, Range range) {
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

        auto ran = Range::toRange(tk.line, previousTokenPosition, tk.column - previousTokenPosition);
        auto ex = inException(ran, parser);
        if (ex.has_value()) {
            tk = {bad_tk, "", tk.line, ex->start.character, ex->end.character - ex->start.character};
        }

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

        int position = previousTokenPosition + tokenSpacing(tk.tk);

        if (firstInLine) {
            if (tk.tk != endl_tk) {
                std::string nexText = (options.insertSpaces) ?
                                      repeatCharacter(' ', level * static_cast<int>(options.tabSize)) :
                                      repeatCharacter('\t', level);

                res->elements.emplace_back(new TextEdit({tk.line, previousTokenPosition},
                                                        {tk.line, tk.column}, nexText));
            }
        } else {
            if (tk.column > position) {
                // If the current token's column is greater than the expected position,
                // add an empty TextEdit to compensate for the missing content
                res->elements.emplace_back(new TextEdit({tk.line, position}, {tk.line, tk.column}, ""));
            } else if (tk.column < position) {
                // If the current token's column is less than the expected position,
                // calculate the necessary indentation and add it as nexText
                std::string nexText = repeatCharacter(' ', position - tk.column);
                res->elements.emplace_back(
                        new TextEdit({tk.line, previousTokenPosition}, {tk.line, previousTokenPosition}, nexText));
            }
        }

        if (tk.tk != endl_tk) {
            previousTokenPosition = tk.column + tk.size;
            firstInLine = false;
        } else {
            previousTokenPosition = 0;
            firstInLine = true;
        }

        if (tk.tk != bad_tk) {
            parser->nextToken();
        }

        if (tk.tk == left_curly_bracket_tk) {
            level++;
        }
    }

    // add end line
    if (options.insertFinalNewline && lastToken.tk != endl_tk) {
        res->elements.emplace_back(new TextEdit({lastToken.line, lastToken.column + lastToken.size},
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

std::string OgreScriptLSP::Formatter::repeatCharacter(char c, const std::size_t times) {
    return std::string(times, c);
}

std::optional<OgreScriptLSP::Range>
OgreScriptLSP::Formatter::inException(OgreScriptLSP::Range range, OgreScriptLSP::Parser *parser) {
    // toDo (gonzalezext)[18.02.24]: this can be more efficient with some kind of algorithm
    for (const auto &e: parser->getExceptions()) {
        if (range.inRange(e.range)) {
            return e.range;
        }
    }
    return std::nullopt;
}

int OgreScriptLSP::Formatter::tokenSpacing(OgreScriptLSP::Token tk) {
    // toDo (gonzalezext)[18.02.24]: tokens missing
    switch (tk) {
        case endl_tk:
        case comma_tk:
            return 0;
        default:
            return 1;
    }
}
