#include "../inc/goto.h"

OgreScriptLSP::ResultBase *OgreScriptLSP::GoTo::goToDefinition(MaterialScriptAst *script, Position position) {
    // toDo (gonzalezext)[03.02.24]: this should be extended to more than one script AST when importing be supported
    auto res = searchInMaterials(script, position);
    if (res.has_value()) {
        return res.value();
    }

    // variables todo: support for goto in variables not implemented

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
    for (auto mat: script->materials) {
        TokenValue parent = mat->parent;
        TokenValue name = mat->name;
        Range rangeParent = {{parent.line, parent.column},
                             {parent.line, parent.column + parent.size - 1}};
        Range rangeName = {{name.line, name.column},
                           {name.line, name.column + name.size - 1}};
        if (name.tk == identifier && rangeName.inRange(position)) {
            // go to definition from same class
            return new Location(script->uri, rangeName);
        }
        if (parent.tk == identifier && rangeParent.inRange(position)) {
            // go to definition from parent
            for (auto mat2: script->materials) {
                if (mat2->name.literal == parent.literal) {
                    TokenValue ftk = mat2->name;
                    Range rangeLocation = {{ftk.line, ftk.column},
                                           {ftk.line, ftk.column + ftk.size - 1}};
                    return new Location(script->uri, rangeLocation);
                }
            }
            return new Location(script->uri, rangeParent);
        }
    }
    return std::nullopt;
}

std::optional<OgreScriptLSP::ResultBase *>
OgreScriptLSP::GoTo::searchInPrograms(OgreScriptLSP::MaterialScriptAst *script, OgreScriptLSP::Position position) {
    for (auto prog: script->programs) {
        Range rangeProg = {{prog->name.line, prog->name.column},
                           {prog->name.line, prog->name.column + prog->name.size - 1}};
        if (rangeProg.inRange(position)) {
            // return same identifier position
            return new Location(script->uri, rangeProg);
        }
    }
    for (auto mat: script->materials) {
        for (auto tech: mat->techniques) {
            for (auto pass: tech->passes) {
                for (auto progRef: pass->programsReferences) {
                    Range rangeProgRef = {{progRef->name.line, progRef->name.column},
                                          {progRef->name.line, progRef->name.column + progRef->name.size - 1}};
                    if (rangeProgRef.inRange(position)) {
                        for (auto prog: script->programs) {
                            if (prog->name.literal == progRef->name.literal) {
                                Range rangeProg = {{prog->name.line, prog->name.column},
                                                   {prog->name.line, prog->name.column + prog->name.size - 1}};
                                return new Location(script->uri, rangeProg);
                            }
                        }

                        // same position in case of not definition found
                        return new Location(script->uri, rangeProgRef);
                    }
                }
            }
        }
    }

    return std::nullopt;
}
