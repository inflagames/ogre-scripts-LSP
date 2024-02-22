#ifndef OGRE_SCRIPTS_LSP_LIB_AST_TREE_H
#define OGRE_SCRIPTS_LSP_LIB_AST_TREE_H

#include <utility>
#include <vector>
#include "token.h"

namespace OgreScriptLSP {

    class AstObject {
    public:
        TokenValue name = {EOF_tk, ""};
        TokenValue parent = {EOF_tk, ""};

        virtual ~AstObject() = default;
    };

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

    class AbstractAst {
    public:
        TokenValue type;
        AstObject *body{};

        ~AbstractAst() {
            delete body;
        }
    };

    class ImportAst {
    public:
        TokenValue imported;
        TokenValue file;
    };

    class SharedParamsAst : public AstObject {
    public:
        std::vector<ParamProgramAst *> params;

        ~SharedParamsAst() override {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
        }
    };

    class ProgramAst : public AstObject {
    public:
        TokenValue type;
        std::vector<ParamProgramAst *> params;
        std::vector<ParamProgramDefaultAst *> defaults;
        std::vector<TokenValue> highLevelProgramsType;

        ~ProgramAst() override {
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

    class MaterialProgramSharedParamAst {
    public:
        TokenValue identifier;
    };

    class MaterialProgramAst {
    public:
        TokenValue name;
        TokenValue type;
        std::vector<MaterialProgramParamAst *> params;
        std::vector<MaterialProgramSharedParamAst *> sharedParams;

        ~MaterialProgramAst() {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
            for (auto ele: sharedParams) {
                delete ele;
            }
            sharedParams.clear();
        }
    };

    class RtShaderParamAst : public ParamAst {
    public:
    };

    class RtShaderAst : public AstObject {
    public:
        std::vector<RtShaderParamAst *> params;

        ~RtShaderAst() override {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
        }
    };

    class TextureUnitParamAst : public ParamAst {
    public:
    };

    class TextureUnitAst : public AstObject {
    public:
        std::vector<TextureUnitParamAst *> params;

        ~TextureUnitAst() override {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
        }
    };

    class ShadowMaterialAst {
    public:
        TokenValue type = {EOF_tk, ""};
        TokenValue reference = {EOF_tk, ""};
    };

    class PassParamAst : public ParamAst {
    };

    class PassAst : public AstObject {
    public:
        std::vector<PassParamAst *> params;
        std::vector<TextureUnitAst *> textures;
        std::vector<RtShaderAst *> shaders;
        std::vector<MaterialProgramAst *> programsReferences;

        ~PassAst() override {
            for (auto ele: params) {
                delete ele;
            }
            params.clear();
            for (auto ele: textures) {
                delete ele;
            }
            textures.clear();
            for (auto ele: programsReferences) {
                delete ele;
            }
            programsReferences.clear();
        }
    };

    class TechniqueAst : public AstObject {
    public:
        std::vector<PassAst *> passes;
        std::vector<PassParamAst *> params;
        std::vector<ShadowMaterialAst *> shadowMaterials;

        ~TechniqueAst() override {
            for (auto ele: passes) {
                delete ele;
            }
            passes.clear();
        }
    };

    class MaterialParamAst : public ParamAst {
    public:
    };

    class MaterialAst : public AstObject {
    public:
        TokenValue symbol = {material_tk, ""};
        std::vector<MaterialParamAst *> params;
        std::vector<TechniqueAst *> techniques;

        ~MaterialAst() override {
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
        std::vector<AbstractAst *> abstracts;
        std::vector<ImportAst *> imports;
        std::vector<SharedParamsAst *> sharedParams;

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
            for (auto ele: abstracts) {
                delete ele;
            }
            abstracts.clear();
            for (auto ele: imports) {
                delete ele;
            }
            imports.clear();
            for (auto ele: sharedParams) {
                delete ele;
            }
            sharedParams.clear();
        }

        std::vector<MaterialAst *> allMaterials() {
            std::vector<MaterialAst *> res;
            res.insert(res.end(), materials.begin(), materials.end());
            for (auto a: abstracts) {
                if (a->type.tk == material_tk) {
                    res.push_back(dynamic_cast<MaterialAst *>(a->body));
                }
            }
            return res;
        }
    };
}

#endif //OGRE_SCRIPTS_LSP_LIB_AST_TREE_H
