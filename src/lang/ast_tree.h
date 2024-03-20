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

        virtual ~ParamAst() = default;
    };

    class ParamProgramAst : public ParamAst {
    };

    class SharedParamsAst : public AstObject {
    public:
        std::vector<std::unique_ptr<ParamProgramAst>> params;
    };

    class ParamProgramDefaultAst : public ParamAst {
    };

    class AbstractAst {
    public:
        TokenValue type;
        std::unique_ptr<AstObject> body{};
    };

    class ImportAst {
    public:
        TokenValue imported;
        TokenValue file;
    };

    class MaterialProgramSharedParamAst {
    public:
        TokenValue identifier;
    };

    class ProgramAst : public AstObject {
    public:
        TokenValue type;
        std::vector<std::unique_ptr<ParamProgramAst>> params;
        std::vector<std::unique_ptr<ParamProgramDefaultAst>> defaults;
        std::vector<TokenValue> highLevelProgramsType;
        std::vector<std::unique_ptr<MaterialProgramSharedParamAst>> sharedParams;
    };

    class MaterialProgramParamAst : public ParamAst {
    public:
    };

    class MaterialProgramAst {
    public:
        TokenValue name;
        TokenValue type;
        std::vector<std::unique_ptr<MaterialProgramParamAst>> params;
        std::vector<std::unique_ptr<MaterialProgramSharedParamAst>> sharedParams;
    };

    class RtShaderParamAst : public ParamAst {
    public:
    };

    class TextureSourceAst : public AstObject {
    public:
        std::vector<std::unique_ptr<RtShaderParamAst>> params;
    };

    class RtShaderAst : public AstObject {
    public:
        std::vector<std::unique_ptr<RtShaderParamAst>> params;
    };

    class SamplerRefAst {
    public:
        TokenValue identifier;
    };

    class TextureUnitParamAst : public ParamAst {
    public:
    };

    class TextureUnitAst : public AstObject {
    public:
        std::vector<std::unique_ptr<TextureUnitParamAst>> params;
        std::vector<std::unique_ptr<RtShaderAst>> shaders;
        std::vector<std::unique_ptr<TextureSourceAst>> textureSources;
        std::vector<std::unique_ptr<SamplerRefAst>> sampleReferences;
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
        std::vector<std::unique_ptr<PassParamAst>> params;
        std::vector<std::unique_ptr<TextureUnitAst>> textures;
        std::vector<std::unique_ptr<RtShaderAst>> shaders;
        std::vector<std::unique_ptr<MaterialProgramAst>> programsReferences;
    };

    class TechniqueParamAst : public ParamAst {
    };

    class TechniqueAst : public AstObject {
    public:
        std::vector<std::unique_ptr<PassAst>> passes;
        std::vector<std::unique_ptr<TechniqueParamAst>> params;
        std::vector<std::unique_ptr<ShadowMaterialAst>> shadowMaterials;
    };

    class MaterialParamAst : public ParamAst {
    };

    class MaterialAst : public AstObject {
    public:
        TokenValue symbol = {material_tk, ""};
        std::vector<std::unique_ptr<MaterialParamAst>> params;
        std::vector<std::unique_ptr<TechniqueAst>> techniques;
    };

    class SamplerParamAst : public ParamAst {
    };

    class SamplerAst : public AstObject {
    public:
        std::vector<std::unique_ptr<SamplerParamAst>> params;
    };

    class MaterialScriptAst {
    public:
        std::string uri;
        std::vector<std::unique_ptr<MaterialAst>> materials;
        std::vector<std::unique_ptr<ProgramAst>> programs;
        std::vector<std::unique_ptr<AbstractAst>> abstracts;
        std::vector<std::unique_ptr<ImportAst>> imports;
        std::vector<std::unique_ptr<SharedParamsAst>> sharedParams;
        std::vector<std::unique_ptr<SamplerAst>> sampler;

        explicit MaterialScriptAst(std::string uri) {
            this->uri = std::move(uri);
        }
    };
}

#endif //OGRE_SCRIPTS_LSP_LIB_AST_TREE_H
