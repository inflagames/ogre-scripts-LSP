//
// Created by guille on 21.01.24.
//

#ifndef JS_AI_PARSER_H
#define JS_AI_PARSER_H

#include "scanner.h"
#include "ast_tree.h"
#include "exceptions.h"
#include "lsp_protocol.h"


namespace OgreScriptLSP {
    class Parser {
    private:
        Scanner *scanner;

        std::vector<TokenValue> tokens;
        int currentToken = 0;

        MaterialScriptAst *script = nullptr;

        std::vector<BaseException> exceptions;

    public:
        Parser();

        ~Parser();

        MaterialScriptAst *getScript() {
            return script;
        }

        std::vector<BaseException> getExceptions() {
            return exceptions;
        }

        void loadScript(const std::string &scriptFile);

        static std::string uriToPath(const std::string &uri);

        void parse();

        ResultArray *formatting(FormattingOptions options, Range range = {{0, 0},
                                               {INT32_MAX, INT32_MAX}});

        static std::string repeatCharacter(char c, int times);

        ResultBase *goToDefinition(Position position);

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

        void nextToken();

        void consumeEndLines();

        bool isEof();

        bool isMainStructure();

        std::vector<TokenValue> objectDefinition(std::string error1, std::string error2);
    };
}


#endif //JS_AI_PARSER_H
