#include "../inc/goto.h"

OgreScriptLSP::ResultBase *OgreScriptLSP::GoTo::goToDefinition(MaterialScriptAst *script, Position position) {
    // toDo (gonzalezext)[03.02.24]: this should be extended to more than one script AST when importing be supported
    auto res = searchInMaterials(script, position);
    if (res.has_value()) {
        return res.value();
    }

    // todo: techniques

    // todo: pass

    // todo: textures_unit

    // todo: variables

    // programs
    res = searchInPrograms(script, position);
    if (res.has_value()) {
        return res.value();
    }

    // by default return same position range
    Range range = {{position.line, position.character},
                   {position.line, position.character}};
    return new Location(script->uri, range);
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


