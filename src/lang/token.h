#ifndef JS_AI_TOKEN_H
#define JS_AI_TOKEN_H

#include "../lsp_protocol.h"

namespace OgreScriptLSP {
    enum Token {
        // language keys
        abstract_tk,
        default_params_tk,
        from_tk,
        import_tk,
        material_tk,
        pass_tk,
        technique_tk,
        texture_unit_tk,
        rtshader_system_tk,
        shared_params_tk,
        shared_params_ref_tk,
        shadow_caster_material_tk,
        shadow_receiver_material_tk,
        texture_source_tk,
        sampler_tk,
        sampler_ref_tk,

        // programs tokens
        fragment_program_tk,
        fragment_program_ref_tk, // ref
        vertex_program_tk,
        vertex_program_ref_tk, // ref
        geometry_program_tk,
        geometry_program_ref_tk, // ref
        tessellation_hull_program_tk,
        tessellation_hull_program_ref_tk, // ref
        tessellation_domain_program_tk,
        tessellation_domain_program_ref_tk, // ref
        compute_program_tk,
        compute_program_ref_tk, // ref

        variable, // $variable_example
        string_literal, // "string literal"
        match_literal, // *match_literal*
        number_literal, // any number
        identifier, // any variable|function|class identifier

        // symbols
        colon_tk, // :
        asterisk_tk, // *
        comma_tk, // ,
        left_curly_bracket_tk, // {
        right_curly_bracket_tk, // }

        // system
        bad_tk,
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
