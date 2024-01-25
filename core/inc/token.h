//
// Created by gonzalezext on 21.01.24.
//

#ifndef JS_AI_TOKEN_H
#define JS_AI_TOKEN_H

namespace OgreScriptLSP {
    enum Token {
        // language keys
        abstract_tk,
        default_params_tk,
        delegate_tk,
        entry_point_tk,
        fragment_program_tk,
        material_tk,
        param_named_tk,
        pass_tk,
        profiles_tk,
        set_tk,
        source_tk,
        technique_tk,
        unified_tk,
        vertex_program_tk,

        int_tk,
        false_tk,
        true_tk,

        variable,
        string_literal,
        match_literal,
        number_literal,
        identifier, // any variable|function|class identifier

        // symbols
        colon_tk, // : X
        semicolon_tk, // ; X todo: probably not needed
        left_parenthesis_tk, // ( X
        right_parenthesis_tk, // ) X
        left_curly_bracket_tk, // { X
        right_curly_bracket_tk, // } X
        left_square_bracket_tk, // [ X
        right_square_bracket_tk, // ] X

        // math
        equals_tk, // = X todo: probably not needed
        plus_tk, // + X todo: probably not needed
        minus_tk, // - X todo: probably not needed
        asterisk_tk, // * X todo: probably not needed
        slash_tk, // / X todo: probably not needed
        tilde_accent_tk, // ~ X todo: probably not needed

        // logic
        exclamation_tk, // ! X todo: probably not needed
        lt_tk, // < X todo: probably not needed
        gt_tk, // > X todo: probably not needed
        ampersand_tk, // & X todo: probably not needed
        vertical_bar_tk, // | X todo: probably not needed

        // system
        EOF_tk, // todo: probably not needed
    };

    struct TokenValue {
        Token tk;
        std::string literal;
    };
}

#endif //JS_AI_TOKEN_H
