//
// Created by gonzalezext on 21.01.24.
//

#ifndef JS_AI_TOKEN_H
#define JS_AI_TOKEN_H

namespace Jsai {
    enum Token {
        // system
        function_tk = 0, // function
        request_tk, // request
        let_tk, // let
        const_tk, // const
        var_tk, // var
        string_literal,
        number_literal,
        if_tk, // if
        else_tk, // else
        for_tk, // for
        do_while_tk, // do
        while_tk, // while
        true_tk, // true
        false_tk, // false
        identifier, // any variable|function|class identifier


        // symbols
        colon_tk, // : X
        semicolon_tk, // ; X
        period_tk, // . X
        comma_tk, // , X
        backslash_tk, // \' X todo: probably not needed
        left_parenthesis_tk, // ( X
        right_parenthesis_tk, // ) X
        left_curly_bracket_tk, // { X
        right_curly_bracket_tk, // } X
        left_square_bracket_tk, // [ X
        right_square_bracket_tk, // ] X

        // string
        double_quote_tk, // " X
        single_quote_tk, // ' X
        back_apostrophe, // ` X

        // math
        equals_tk, // = X
        plus_tk, // + X
        minus_tk, // - X
        asterisk_tk, // * X
        slash_tk, // / X
        tilde_accent_tk, // ~ X

        // logic
        exclamation_tk, // ! X
        lt_tk, // < X
        gt_tk, // > X
        ampersand_tk, // & X
        vertical_bar_tk, // | X

        // system
        EOF_tk,
    };

    struct TokenValue {
        Token tk;
        std::string literal;
    };
}

#endif //JS_AI_TOKEN_H
