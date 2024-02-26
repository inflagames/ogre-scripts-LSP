#include "goto.h"

std::unique_ptr<OgreScriptLSP::ResultBase> OgreScriptLSP::GoTo::goToDefinition(OgreScriptLSP::MaterialScriptAst *script,
                                                                               std::unique_ptr<std::map<std::pair<int, std::string>, TokenValue>> declarations,
                                                                               const Position position) {
    auto el = search(script, position);
    if (el.has_value() && declarations->contains(el.value())) {
        return std::make_unique<Location>(script->uri, declarations->at(el.value()).toRange());
    }

    // by default return same position range
    Range range = {{position.line, position.character},
                   {position.line, position.character}};

    return std::make_unique<Location>(script->uri, range);
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::search(OgreScriptLSP::MaterialScriptAst *script, OgreScriptLSP::Position position) {
    for (auto m: script->materials) {
        auto r = searchMaterial(m, position);
        if (r.has_value()) {
            return r;
        }
    }

    for (auto a: script->abstracts) {
        auto r = searchAbstract(a, position);
        if (r.has_value()) {
            return r;
        }
    }

    for (auto p: script->programs) {
        auto r = searchProgram(p, position);
        if (r.has_value()) {
            return r;
        }
    }

    for (auto sp: script->sharedParams) {
        auto r = searchSharedParam(sp, position);
        if (r.has_value()) {
            return r;
        }
    }

    for (auto s: script->sampler) {
        auto r = searchSampler(s, position);
        if (r.has_value()) {
            return r;
        }
    }

    return std::nullopt;
}

std::optional<std::pair<int, std::string>> OgreScriptLSP::GoTo::searchSampler(SamplerAst *sampler, Position position) {
    auto r = searchInObject(sampler, position, SAMPLER_BLOCK);
    if (r.has_value()) {
        return r;
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchAbstract(OgreScriptLSP::AbstractAst *abstract, OgreScriptLSP::Position position) {
    switch (abstract->type.tk) {
        case material_tk: {
            return searchMaterial(dynamic_cast<MaterialAst *>(abstract->body), position);
        }
        case technique_tk: {
            return searchTechnique(dynamic_cast<TechniqueAst *>(abstract->body), position);
        }
        case pass_tk: {
            return searchPass(dynamic_cast<PassAst *>(abstract->body), position);
        }
        case texture_unit_tk: {
            return searchTexture(dynamic_cast<TextureUnitAst *>(abstract->body), position);
        }
        case rtshader_system_tk: {
            return searchRtShader(dynamic_cast<RtShaderAst *>(abstract->body), position);
        }
        case texture_source_tk: {
            return searchTextureSource(dynamic_cast<TextureSourceAst *>(abstract->body), position);
        }
        default:
            return std::nullopt;
    }
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchMaterial(OgreScriptLSP::MaterialAst *material, OgreScriptLSP::Position position) {
    auto o = searchInObject(material, position, MATERIAL_BLOCK);
    if (o.has_value()) {
        return o;
    }
    for (auto t: material->techniques) {
        auto r = searchTechnique(t, position);
        if (r.has_value()) {
            return r;
        }
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchTechnique(OgreScriptLSP::TechniqueAst *technique, OgreScriptLSP::Position position) {
    auto o = searchInObject(technique, position, TECHNIQUE_BLOCK);
    if (o.has_value()) {
        return o;
    }
    for (auto p: technique->passes) {
        auto r = searchPass(p, position);
        if (r.has_value()) {
            return r;
        }
    }
    for (auto s: technique->shadowMaterials) {
        auto r = searchShadowMat(s, position);
        if (r.has_value()) {
            return r;
        }
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchShadowMat(OgreScriptLSP::ShadowMaterialAst *shadowMat, OgreScriptLSP::Position position) {
    if (shadowMat->reference.toRange().inRange(position)) {
        return std::make_pair(MATERIAL_BLOCK, shadowMat->reference.literal);
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchPass(OgreScriptLSP::PassAst *pass, OgreScriptLSP::Position position) {
    auto o = searchInObject(pass, position, PASS_BLOCK);
    if (o.has_value()) {
        return o;
    }
    for (auto t: pass->textures) {
        auto r = searchTexture(t, position);
        if (r.has_value()) {
            return r;
        }
    }
    for (auto pr: pass->programsReferences) {
        auto r = searchProgramRef(pr, position);
        if (r.has_value()) {
            return r;
        }
    }
    for (auto rt: pass->shaders) {
        auto r = searchRtShader(rt, position);
        if (r.has_value()) {
            return r;
        }
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchTexture(OgreScriptLSP::TextureUnitAst *texture, OgreScriptLSP::Position position) {
    auto o = searchInObject(texture, position, TEXTURE_UNIT_BLOCK);
    if (o.has_value()) {
        return o;
    }
    for (auto rt: texture->shaders) {
        auto r = searchRtShader(rt, position);
        if (r.has_value()) {
            return r;
        }
    }
    for (auto ts: texture->textureSources) {
        auto r = searchTextureSource(ts, position);
        if (r.has_value()) {
            return r;
        }
    }
    for (auto s: texture->sampleReferences) {
        auto r = searchSamplerRef(s, position);
        if (r.has_value()) {
            return r;
        }
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchSamplerRef(SamplerRefAst *samplerRef, Position position) {
    if (samplerRef->identifier.toRange().inRange(position)) {
        return std::make_pair(SAMPLER_BLOCK, samplerRef->identifier.literal);
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchTextureSource(OgreScriptLSP::TextureSourceAst *textureSource,
                                         OgreScriptLSP::Position position) {
    auto o = searchInObject(textureSource, position, TEXTURE_SOURCE_BLOCK);
    if (o.has_value()) {
        return o;
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchRtShader(OgreScriptLSP::RtShaderAst *shader, OgreScriptLSP::Position position) {
    auto o = searchInObject(shader, position, RTSHADER_BLOCK);
    if (o.has_value()) {
        return o;
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchSharedParam(OgreScriptLSP::SharedParamsAst *sharedParam, OgreScriptLSP::Position position) {
    auto o = searchInObject(sharedParam, position, SHARED_PARAMS_BLOCK);
    if (o.has_value()) {
        return o;
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchProgram(OgreScriptLSP::ProgramAst *program, OgreScriptLSP::Position position) {
    int type = Parser::getProgramBlockIdk(program->type.tk);
    auto o = searchInObject(program, position, type);
    if (o.has_value()) {
        return o;
    }
    for (const auto sp: program->sharedParams) {
        auto r = searchSharedParamRef(sp, position);
        if (r.has_value()) {
            return r;
        }
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchProgramRef(OgreScriptLSP::MaterialProgramAst *programRef, OgreScriptLSP::Position position) {
    int type = Parser::getProgramBlockIdk(programRef->type.tk);
    if (programRef->name.toRange().inRange(position)) {
        return std::make_pair(type, programRef->name.literal);
    }
    for (const auto sp: programRef->sharedParams) {
        auto r = searchSharedParamRef(sp, position);
        if (r.has_value()) {
            return r;
        }
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchSharedParamRef(OgreScriptLSP::MaterialProgramSharedParamAst *sharedParam,
                                          OgreScriptLSP::Position position) {
    if (sharedParam->identifier.toRange().inRange(position)) {
        return std::make_pair(SHARED_PARAMS_BLOCK, sharedParam->identifier.literal);
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchInObject(OgreScriptLSP::AstObject *object, OgreScriptLSP::Position position, int type) {
    if (object->name.toRange().inRange(position)) {
        return std::make_pair(type, object->name.literal);
    }
    if (object->parent.toRange().inRange(position)) {
        return std::make_pair(type, object->parent.literal);
    }
    return std::nullopt;
}
