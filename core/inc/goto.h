#ifndef OGRE_SCRIPTS_LSP_LIB_GOTO_H
#define OGRE_SCRIPTS_LSP_LIB_GOTO_H


#include "lsp_protocol.h"
#include "token.h"
#include "parser.h"
#include "../inc/ast_tree.h"

namespace OgreScriptLSP {
    class GoTo {
    public:
        /**
         * @brief Finds the definition associated with the given position in a material script.
         *
         * @param script A unique pointer to the AST of the material script.
         * @param declarations A unique pointer to a map containing declarations in the script, mapped by position and name.
         * @param position The position at which the definition is sought.
         * @return A unique pointer to the result of the definition search.
         *
         * This function searches for the definition associated with the given position in a material script.
         * It returns a pointer to the result of the search, which contains relevant information about the found definition.
         * If no definition is found at the given position, the returned pointer is null.
         */
        static std::unique_ptr<OgreScriptLSP::ResultBase>
        goToDefinition(OgreScriptLSP::MaterialScriptAst *script,
                       std::unique_ptr<std::map<std::pair<int, std::string>, TokenValue>> declarations,
                       Position position);

    private:
        /**
         * @brief Searches for a token at the given position in a material script AST.
         *
         * @param script A unique pointer to the AST of the material script.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given material script AST.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>>
        search(OgreScriptLSP::MaterialScriptAst *script, OgreScriptLSP::Position position);

        /**
         * @brief Searches for a token at the given position in an abstract syntax tree (AST).
         *
         * @param abstract Pointer to the abstract syntax tree (AST) to be searched.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given abstract syntax tree.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>> searchAbstract(AbstractAst *abstract, Position position);

        /**
         * @brief Searches for a token at the given position in a material script abstract syntax tree (AST).
         *
         * @param script Pointer to the material script abstract syntax tree (AST) to be searched.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given material script abstract syntax tree.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>> searchMaterial(MaterialAst *script, Position position);

        /**
         * @brief Searches for a token at the given position in a technique script abstract syntax tree (AST).
         *
         * @param script Pointer to the technique script abstract syntax tree (AST) to be searched.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given technique script abstract syntax tree.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>> searchTechnique(TechniqueAst *script, Position position);

        static std::optional<std::pair<int, std::string>>
        searchShadowMat(ShadowMaterialAst *shadowMat, Position position);

        static std::optional<std::pair<int, std::string>>
        searchSharedParam(SharedParamsAst *sharedParam, Position position);

        static std::optional<std::pair<int, std::string>>
        searchSharedParamRef(MaterialProgramSharedParamAst *sharedParam, Position position);

        static std::optional<std::pair<int, std::string>>
        searchTextureSource(TextureSourceAst *textureSource, Position position);

        /**
         * @brief Searches for a token at the given position in a pass script abstract syntax tree (AST).
         *
         * @param pass Pointer to the pass script abstract syntax tree (AST) to be searched.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given pass script abstract syntax tree.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>> searchPass(PassAst *pass, Position position);

        /**
         * @brief Searches for a token at the given position in a texture unit script abstract syntax tree (AST).
         *
         * @param texture Pointer to the texture unit script abstract syntax tree (AST) to be searched.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given texture unit script abstract syntax tree.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>> searchTexture(TextureUnitAst *texture, Position position);

        static std::optional<std::pair<int, std::string>> searchRtShader(RtShaderAst *shader, Position position);

        /**
         * @brief Searches for a token at the given position in a program script abstract syntax tree (AST).
         *
         * @param program Pointer to the program script abstract syntax tree (AST) to be searched.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given program script abstract syntax tree.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>> searchProgram(ProgramAst *program, Position position);

        /**
         * @brief Searches for a token at the given position in a material program reference abstract syntax tree (AST).
         *
         * @param programRef Pointer to the material program reference abstract syntax tree (AST) to be searched.
         * @param position The position at which the token is to be searched.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given material program reference abstract syntax tree.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>>
        searchProgramRef(MaterialProgramAst *programRef, Position position);

        /**
         * @brief Searches for a token at the given position in an abstract syntax tree (AST) object of a specified type.
         *
         * @param object Pointer to the AST object to be searched.
         * @param position The position at which the token is to be searched.
         * @param type The type of the AST object.
         * @return An optional pair containing the line number and token string found at the position, if any.
         *
         * This function searches for a token at the specified position within the given AST object.
         * It returns an optional pair containing the line number and token string found at the position, if any.
         * If no token is found at the given position, an empty optional is returned.
         */
        static std::optional<std::pair<int, std::string>>
        searchInObject(AstObject *object, Position position, int type);
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_GOTO_H
