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

std::optional<OgreScriptLSP::ResultBase *>
OgreScriptLSP::GoTo::searchInMaterials(MaterialScriptAst *script, Position position) {
    for (auto mat: script->allMaterials()) {
        auto res = searchInMaterial(mat, position);
        if (res.has_value()) {
            auto result = searchMaterialDefinition(script, res->literal);
            if (result.has_value()) {
                // return definition position
                return result;
            }

            return new Location(script->uri, Range::toRange(res->line, res->column, res->size - 1));
        }
    }
    return std::nullopt;
}

std::optional<OgreScriptLSP::TokenValue>
OgreScriptLSP::GoTo::searchInMaterial(MaterialAst *mat,
                                      OgreScriptLSP::Position position) {
    TokenValue parent = mat->parent;
    TokenValue name = mat->name;
    Range rangeParent = {{parent.line, parent.column},
                         {parent.line, parent.column + parent.size - 1}};
    Range rangeName = {{name.line, name.column},
                       {name.line, name.column + name.size - 1}};
    if (name.tk == identifier && rangeName.inRange(position)) {
        // go to definition from same class
        return name;
    }
    if (parent.tk == identifier && rangeParent.inRange(position)) {
        return parent;
    }
    return std::nullopt;
}

std::optional<OgreScriptLSP::ResultBase *>
OgreScriptLSP::GoTo::searchMaterialDefinition(OgreScriptLSP::MaterialScriptAst *script,
                                              const std::string &parentLiteral) {
    for (auto mat2: script->allMaterials()) {
        if (mat2->name.literal == parentLiteral) {
            TokenValue ftk = mat2->name;
            Range rangeLocation = {{ftk.line, ftk.column},
                                   {ftk.line, ftk.column + ftk.size - 1}};
            return new Location(script->uri, rangeLocation);
        }
    }
    return std::nullopt;
}

std::optional<OgreScriptLSP::ResultBase *>
OgreScriptLSP::GoTo::searchInPrograms(OgreScriptLSP::MaterialScriptAst *script, OgreScriptLSP::Position position) {
    // search by programs identifiers
    for (auto prog: script->programs) {
        Range rangeProg = {{prog->name.line, prog->name.column},
                           {prog->name.line, prog->name.column + prog->name.size - 1}};
        if (rangeProg.inRange(position)) {
            // return same identifier position
            return new Location(script->uri, rangeProg);
        }
    }

    // search programs references
    for (auto mat: script->materials) {
        auto res = searchProgramInMaterial(mat, position);
        if (res.has_value()) {
            return searchProgramDefinition(script, res.value());
        }
    }

    // search programs in abstracts
    for (auto abs: script->abstracts) {
        switch (abs->type.tk) {
            case material_tk: {
                auto res = searchProgramInMaterial(dynamic_cast<MaterialAst *>(abs->body), position);
                if (res.has_value()) {
                    return searchProgramDefinition(script, res.value());
                }
            }
            case technique_tk: {
                auto res = searchProgramInTechnique(dynamic_cast<TechniqueAst *>(abs->body), position);
                if (res.has_value()) {
                    return searchProgramDefinition(script, res.value());
                }
            }
            case pass_tk: {
                auto res = searchProgramInPasses(dynamic_cast<PassAst *>(abs->body), position);
                if (res.has_value()) {
                    return searchProgramDefinition(script, res.value());
                }
            }
            default:
                break;
        }
    }

    return std::nullopt;
}

std::optional<OgreScriptLSP::ResultBase *>
OgreScriptLSP::GoTo::searchProgramDefinition(OgreScriptLSP::MaterialScriptAst *script, const TokenValue &programTk) {
    for (auto prog: script->programs) {
        if (prog->name.literal == programTk.literal) {
            Range rangeProg = Range::toRange(prog->name.line, prog->name.column, prog->name.size - 1);
            return new Location(script->uri, rangeProg);
        }
    }
    Range rangeProgRef = Range::toRange(programTk.line, programTk.column, programTk.size - 1);
    return new Location(script->uri, rangeProgRef);
}

std::optional<OgreScriptLSP::TokenValue>
OgreScriptLSP::GoTo::searchProgramInMaterial(MaterialAst *mat,
                                             OgreScriptLSP::Position position) {
    for (auto tech: mat->techniques) {
        auto res = searchProgramInTechnique(tech, position);
        if (res.has_value()) {
            return res;
        }
    }
    return std::nullopt;
}

std::optional<OgreScriptLSP::TokenValue>
OgreScriptLSP::GoTo::searchProgramInTechnique(OgreScriptLSP::TechniqueAst *tech, OgreScriptLSP::Position position) {
    for (auto pass: tech->passes) {
        auto res = searchProgramInPasses(pass, position);
        if (res.has_value()) {
            return res;
        }
    }
    return std::nullopt;
}

std::optional<OgreScriptLSP::TokenValue>
OgreScriptLSP::GoTo::searchProgramInPasses(OgreScriptLSP::PassAst *pass, OgreScriptLSP::Position position) {
    for (auto progRef: pass->programsReferences) {
        Range rangeProgRef = Range::toRange(progRef->name.line, progRef->name.column, progRef->name.size - 1);
        if (rangeProgRef.inRange(position)) {
            return progRef->name;
        }
    }
    return std::nullopt;
}


