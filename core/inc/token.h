#ifndef JS_AI_TOKEN_H
#define JS_AI_TOKEN_H

#include "lsp_protocol.h"

namespace OgreScriptLSP {
    enum Token {
        // language keys
        abstract_tk,
        default_params_tk,
        entry_point_tk,
        fragment_program_tk,
        fragment_program_ref_tk,
        from_tk,
        import_tk,
        material_tk,
        pass_tk,
        profiles_tk,
        technique_tk,
        texture_unit_tk,
        vertex_program_tk,
        vertex_program_ref_tk,

        variable, // $variable_example
        string_literal, // "string literal"
        match_literal, // *match_literal*
        number_literal, // any number
        identifier, // any variable|function|class identifier

        // symbols
        colon_tk, // : X
        asterisk_tk, // *
        left_curly_bracket_tk, // { X
        right_curly_bracket_tk, // } X

        // system
        endl_tk,
        EOF_tk,
    };

    struct TokenValue {
        Token tk;
        std::string literal;
        int line;
        int column;
        int size = 1;

        Range toRange() {
            return Range{
                    {line, column},
                    {line, column + size}
            };
        }
    };
}

#endif //JS_AI_TOKEN_H
