#include "semantic_tokens.h"

OgreScriptLSP::ResultBase *
OgreScriptLSP::SemanticToken::getSemanticTokens(OgreScriptLSP::Parser *parser, Range range) {
    auto *res = new SemanticTokens();

    res->data.insert(std::end(res->data), parser->getScanner()->comments.begin(), parser->getScanner()->comments.end());

    parser->initSwap();
    while (!parser->isEof()) {
        auto tk = parser->getToken();
        switch (parser->getToken().tk) {
            case material_tk:
            case shared_params_tk:
            case technique_tk:
            case vertex_program_ref_tk:
            case fragment_program_ref_tk:
            case tessellation_hull_program_ref_tk:
            case compute_program_ref_tk:
            case geometry_program_ref_tk:
            case tessellation_domain_program_ref_tk:
            case texture_unit_tk:
            case rtshader_system_tk:
            case texture_source_tk:
            case sampler_tk:
            case pass_tk: {
                res->data.push_back({(uint32_t) tk.line, (uint32_t) tk.column,
                                     (uint32_t) tk.size, 0, 0});
                break;
            }
            case shared_params_ref_tk:
            case sampler_ref_tk:
            case shadow_caster_material_tk:
            case shadow_receiver_material_tk: {
                res->data.push_back({(uint32_t) tk.line, (uint32_t) tk.column,
                                     (uint32_t) tk.size, 4, 0});
                break;
            }
            case number_literal:
                res->data.push_back({(uint32_t) tk.line, (uint32_t) tk.column,
                                     (uint32_t) tk.size, 5, 0});
                break;
            case vertex_program_tk:
            case geometry_program_tk:
            case tessellation_hull_program_tk:
            case tessellation_domain_program_tk:
            case compute_program_tk:
            case fragment_program_tk: {
                res->data.push_back({(uint32_t) tk.line, (uint32_t) tk.column,
                                     (uint32_t) tk.size, 2, 0});
                break;
            }
            default:
                break;
        }

        parser->nextToken();
    }

    return res;
}
