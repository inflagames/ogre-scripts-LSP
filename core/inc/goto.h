#ifndef OGRE_SCRIPTS_LSP_LIB_GOTO_H
#define OGRE_SCRIPTS_LSP_LIB_GOTO_H


#include "lsp_protocol.h"
#include "token.h"
#include "parser.h"
#include "../inc/ast_tree.h"

namespace OgreScriptLSP {
    class GoTo {
    public:
        static std::unique_ptr<OgreScriptLSP::ResultBase> goToDefinition(const std::unique_ptr<OgreScriptLSP::MaterialScriptAst>& script,
                                                               std::unique_ptr<std::map<std::pair<int, std::string>, TokenValue>> declarations,
                                                               Position position) ;

    private:
        static std::optional<std::pair<int, std::string>> search(const std::unique_ptr<OgreScriptLSP::MaterialScriptAst>& script, OgreScriptLSP::Position position);

        static std::optional<std::pair<int, std::string>> searchAbstract(AbstractAst *abstract, Position position);

        static std::optional<std::pair<int, std::string>> searchMaterial(MaterialAst *script, Position position);

        static std::optional<std::pair<int, std::string>> searchTechnique(TechniqueAst *script, Position position);

        static std::optional<std::pair<int, std::string>> searchPass(PassAst *pass, Position position);

        static std::optional<std::pair<int, std::string>> searchTexture(TextureUnitAst *texture, Position position);

        static std::optional<std::pair<int, std::string>> searchProgram(ProgramAst *program, Position position);

        static std::optional<std::pair<int, std::string>>
        searchProgramRef(MaterialProgramAst *programRef, Position position);

        static std::optional<std::pair<int, std::string>>
        searchInObject(AstObject *object, Position position, int type);
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_GOTO_H
