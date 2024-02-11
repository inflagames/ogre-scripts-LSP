#ifndef OGRE_SCRIPTS_LSP_LIB_GOTO_H
#define OGRE_SCRIPTS_LSP_LIB_GOTO_H


#include "lsp_protocol.h"
#include "token.h"
#include "parser.h"
#include "../inc/ast_tree.h"

namespace OgreScriptLSP {
    class GoTo {
    public:
        static ResultBase *
        goToDefinition(MaterialScriptAst *script,
                       std::map<std::pair<int, std::string>, TokenValue> declarations,
                       Position position);

    private:
        static std::optional<std::pair<int, std::string>>
        search(MaterialScriptAst *script, Position position);

        static std::optional<std::pair<int, std::string>>
        searchAbstract(AbstractAst *abstract, Position position);

        static std::optional<std::pair<int, std::string>>
        searchMaterial(MaterialAst *script, Position position);

        static std::optional<std::pair<int, std::string>>
        searchTechnique(TechniqueAst *script, Position position);

        static std::optional<std::pair<int, std::string>>
        searchPass(PassAst *pass, Position position);

        static std::optional<std::pair<int, std::string>>
        searchTexture(TextureUnitAst *texture, Position position);

        static std::optional<std::pair<int, std::string>>
        searchProgram(ProgramAst *program, Position position);

        static std::optional<std::pair<int, std::string>>
        searchProgramRef(MaterialProgramAst *programRef, Position position);

        static std::optional<std::pair<int, std::string>>
        searchInObject(AstObject *object, Position position, int type);

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
