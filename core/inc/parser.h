//
// Created by guille on 21.01.24.
//

#ifndef JS_AI_PARSER_H
#define JS_AI_PARSER_H

#include "scanner.h"
#include "ast_tree.h"

namespace OgreScriptLSP {
    class Parser {
    private:
        Scanner *scanner;

        std::vector<TokenValue> tokens;
        int currentToken = 0;

        MaterialScriptAst *script = nullptr;

    public:
        Parser();

        MaterialScriptAst * getScript() {
            return script;
        }

        void loadScript(const std::string &scriptFile);

        void parse();

        void program(MaterialScriptAst *script);

        void programOpt(ProgramAst *program);

        void programBody(ProgramAst *program);

        void programDefaults(ProgramAst *program);

        void paramsLine(ParamAst *params);

        /**
         * current token
         */
        TokenValue getToken();

        void nextTokenAndConsumeEndLines();

        void nextToken();

        void consumeEndLines();
    };
}


#endif //JS_AI_PARSER_H
