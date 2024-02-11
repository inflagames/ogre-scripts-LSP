#ifndef JS_AI_PARSER_H
#define JS_AI_PARSER_H

#include "scanner.h"
#include "ast_tree.h"
#include "exceptions.h"
#include "lsp_protocol.h"
#include "goto.h"

#define MATERIAL_BLOCK 1
#define PROGRAM_VERTEX_BLOCK 2
#define PROGRAM_FRAGMENT_BLOCK 3
#define TECHNIQUE_BLOCK 4
#define PASS_BLOCK 5
#define TEXTURE_UNIT_BLOCK 6

namespace OgreScriptLSP {
    class Parser {
    private:
        Scanner *scanner;

        std::vector<TokenValue> tokens;
        int currentToken = 0;

        MaterialScriptAst *script = nullptr;
        std::map<std::pair<int, std::string>, TokenValue> declarations;

        std::vector<BaseException> exceptions;

    public:
        std::string uri;

        Parser();

        ~Parser();

        MaterialScriptAst *getScript() {
            return script;
        }

        [[nodiscard]] const std::map<std::pair<int, std::string>, TokenValue> &getDeclarations() const;

        std::vector<BaseException> getExceptions() {
            return exceptions;
        }

        void loadScript(const std::string &uri, const std::string &code = "");

        static std::string uriToPath(const std::string &uri);

        void parse(const std::string &uri);

        void parse();

        // IMPORT STATEMENT
        void importBlock(MaterialScriptAst *scriptAst);

        // ABSTRACT STATEMENT
        void abstract(MaterialScriptAst *scriptAst);

        // PROGRAM STATEMENT
        void program(MaterialScriptAst *scriptAst);

        void programOpt(ProgramAst *program);

        void programBody(ProgramAst *program);

        void programDefaults(ProgramAst *program);

        // MATERIAL STATEMENT
        void material(MaterialScriptAst *scriptAst);

        void materialBody(MaterialAst *material);

        void materialTechnique(MaterialAst *material);

        void materialTechniqueBody(TechniqueAst *technique);

        void materialPass(TechniqueAst *technique);

        void materialPassName(PassAst *pass);

        void materialPassBody(PassAst *pass);

        void materialTexture(PassAst *pass);

        void materialTextureBody(TextureUnitAst *texture);

        void materialProgramRef(PassAst *pass);

        void materialProgramRefBody(MaterialProgramAst *programRef);

        // THIS IS COMMON SECTION
        void paramsLine(ParamAst *params);

        /**
         * current token
         */
        TokenValue getToken();

        void recuperateLine();

        void nextTokenAndConsumeEndLines();

        void consumeOpenCurlyBracket();

        void consumeCloseCurlyBracket();

        void consumeToken(Token token, std::string errorMessage, bool consumeEndLines = false);

        void nextToken();

        void consumeEndLines();

        bool isEof();

        void initSwap();

        void registerDeclaration(AstObject *object, int type);

        bool isMainStructure();

        void
        objectDefinition(AstObject *astObject, std::string error1, std::string error2, bool notTopLevelObject = false);
    };
}


#endif //JS_AI_PARSER_H
