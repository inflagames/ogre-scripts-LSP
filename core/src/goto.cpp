#include "../inc/goto.h"

OgreScriptLSP::ResultBase *OgreScriptLSP::GoTo::goToDefinition(MaterialScriptAst *script,
                                                               std::map<std::pair<int, std::string>, TokenValue> declarations,
                                                               Position position) {
    auto el = search(script, position);
    if (el.has_value() && declarations.contains(el.value())) {
        return new Location(script->uri, declarations[el.value()].toRange());
    }

    // by default return same position range
    Range range = {{position.line, position.character},
                   {position.line, position.character}};
    return new Location(script->uri, range);
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
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchTexture(OgreScriptLSP::TextureUnitAst *texture, OgreScriptLSP::Position position) {
    auto o = searchInObject(texture, position, TEXTURE_UNIT_BLOCK);
    if (o.has_value()) {
        return o;
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchProgram(OgreScriptLSP::ProgramAst *program, OgreScriptLSP::Position position) {
    int type = program->type == ProgramAst::vertex ? PROGRAM_VERTEX_BLOCK : PROGRAM_FRAGMENT_BLOCK;
    auto o = searchInObject(program, position, type);
    if (o.has_value()) {
        return o;
    }
    return std::nullopt;
}

std::optional<std::pair<int, std::string>>
OgreScriptLSP::GoTo::searchProgramRef(OgreScriptLSP::MaterialProgramAst *programRef, OgreScriptLSP::Position position) {
    int type = programRef->type.tk == vertex_program_ref_tk ? PROGRAM_VERTEX_BLOCK : PROGRAM_FRAGMENT_BLOCK;
    if (programRef->name.toRange().inRange(position)) {
        return std::make_pair(type, programRef->name.literal);
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
