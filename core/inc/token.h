//
// Created by gonzalezext on 21.01.24.
//

#ifndef JS_AI_TOKEN_H
#define JS_AI_TOKEN_H

namespace Jsai {
    enum Token {
        // system
        break_tk, // break
        case_tk, // case
        catch_tk, // catch
//        class_tk, // todo: not in plan
        const_tk, // const
        continue_tk, //
        default_tk, //
        delete_tk, //
        do_while_tk, // do
        else_tk, // else
//        export_tk,
//        extends,
        false_tk, // false
//        finally_tk,
        for_tk, // for
        function_tk, // function
        if_tk, // if
//        import_tk,
        in_tk,
        instanceof_tk,
        let_tk,
        new_tk,
        null_tk,
        return_tk,
//        super_tk
        switch_tk,
        this_tk,
//        throw_tk,
        true_tk, // true
//        try_tk,
        typeof_tk,
        var_tk, // var
//        void_tk,
        while_tk, // while
        with_tk,

        string_literal,
        number_literal,
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
        double_quote_tk, // " X todo: probably not needed
        single_quote_tk, // ' X todo: probably not needed
        back_apostrophe, // ` X todo: probably not needed

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
        EOF_tk, // todo: probably not needed
    };

    struct TokenValue {
        Token tk;
        std::string literal;
    };
}

#endif //JS_AI_TOKEN_H
