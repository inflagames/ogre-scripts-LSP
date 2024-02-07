//
// Created by gonzalezext on 27.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_AST_TREE_H
#define OGRE_SCRIPTS_LSP_LIB_AST_TREE_H

#include <utility>
#include <vector>

namespace OgreScriptLSP {

    class ParamAst {
    public:
        std::vector<TokenValue> items;

        ~ParamAst() {
            items.clear();
        }
    };

    class ParamProgramAst : public ParamAst {
    public:
    };

    class ParamProgramDefaultAst : public ParamAst {
    public:
    };

    class ProgramAst {
    public:
        std::vector<ParamProgramAst *> params;
        std::vector<ParamProgramDefaultAst *> defaults;
        TokenValue name;
        std::vector<TokenValue> highLevelProgramsType;

        enum ProgramType {
            fragment,
            vertex,
        } type;

        ~ProgramAst() {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
            for (auto ele: defaults) {
                delete ele;
            }
            defaults.clear();
            highLevelProgramsType.clear();
        }
    };

    class MaterialProgramParamAst : public ParamAst {
    public:
    };

    class MaterialProgramAst {
    public:
        TokenValue name;
        TokenValue type;
        std::vector<MaterialProgramParamAst *> params;

        ~MaterialProgramAst() {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
        }
    };

    class TextureUnitParamAst : public ParamAst {
    public:
    };

    class TextureUnitAst {
    public:
        TokenValue name;
        std::vector<TextureUnitParamAst *> params;

        ~TextureUnitAst() {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
        }
    };

    class PassParamAst : public ParamAst {
    public:
    };

    class PassAst {
    public:
        TokenValue name;
        TokenValue parent;
        std::vector<PassParamAst *> params;
        std::vector<TextureUnitAst *> textures;
        std::vector<MaterialProgramAst *> programsReferences;

        ~PassAst() {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
        }
    };

    class TechniqueAst {
    public:
        std::vector<PassAst *> passes;

        ~TechniqueAst() {
            for (auto ele: passes) {
                delete ele;
            }
            passes.clear();
        }
    };

    class MaterialParamAst : public ParamAst {
    public:
    };

    class MaterialAst {
    public:
        TokenValue name;
        TokenValue parent;
        std::vector<MaterialParamAst *> params;
        std::vector<TechniqueAst *> techniques;

        ~MaterialAst() {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
            for (auto ele: techniques) {
                delete ele;
            }
            techniques.clear();
        }
    };

    class MaterialScriptAst {
    public:
        std::string uri;
        std::vector<MaterialAst *> materials;
        std::vector<ProgramAst *> programs;

        explicit MaterialScriptAst(std::string uri) {
            this->uri = std::move(uri);
        }

        ~MaterialScriptAst() {
            for (auto ele: materials) {
                delete ele;
            }
            materials.clear();
            for (auto ele: programs) {
                delete ele;
            }
            programs.clear();
        }
    };
}

#endif //OGRE_SCRIPTS_LSP_LIB_AST_TREE_H