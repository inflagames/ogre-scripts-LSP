#ifndef OGRE_SCRIPTS_LSP_LIB_GOTO_H
#define OGRE_SCRIPTS_LSP_LIB_GOTO_H


#include "lsp_protocol.h"
#include "token.h"
#include "../inc/ast_tree.h"

namespace OgreScriptLSP {
    class GoTo {
    public:
        static ResultBase *goToDefinition(MaterialScriptAst *script, Position position);

    private:
        static std::optional<OgreScriptLSP::ResultBase *>
        searchInMaterials(MaterialScriptAst *script, Position position);

        static std::optional<OgreScriptLSP::TokenValue>
        searchInMaterial(MaterialAst *mat, Position position);

        static std::optional<OgreScriptLSP::ResultBase *>
        searchMaterialDefinition(OgreScriptLSP::MaterialScriptAst *script,
                                 const std::string &parentLiteral);

        static std::optional<OgreScriptLSP::ResultBase *>
        searchInPrograms(OgreScriptLSP::MaterialScriptAst *script, OgreScriptLSP::Position position);

        static std::optional<OgreScriptLSP::ResultBase *>
        searchProgramDefinition(OgreScriptLSP::MaterialScriptAst *script,
                                const TokenValue &programTk);

        static std::optional<OgreScriptLSP::TokenValue>
        searchProgramInMaterial(MaterialAst *mat, OgreScriptLSP::Position position);

        static std::optional<OgreScriptLSP::TokenValue>
        searchProgramInTechnique(TechniqueAst *tech, OgreScriptLSP::Position position);

        static std::optional<OgreScriptLSP::TokenValue>
        searchProgramInPasses(PassAst *pass, OgreScriptLSP::Position position);
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_GOTO_H
