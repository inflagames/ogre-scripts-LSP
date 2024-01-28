//
// Created by guille on 21.01.24.
//

#ifndef JS_AI_PARSER_H
#define JS_AI_PARSER_H

#include "scanner.h"
#include "ast_tree.h"
#include "exceptions.h"

#define PROGRAM_NAME_MISSION "Error with program name identifier"
#define CURLY_BRACKET_START_MISSING "Missing start curly bracket"
#define CURLY_BRACKET_END_MISSING "Missing curly bracket"
#define PROGRAM_HIGH_LEVEL_MISSING "Missing high level program definition"
#define NOT_VALID_PROGRAM_PARAM "Not valid program param"

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

        MaterialScriptAst *getScript() {
            return script;
        }

        std::vector<BaseException> getExceptions() {
            return exceptions;
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

        void recuperateLine();

        void nextTokenAndConsumeEndLines();

        void nextToken();

        void consumeEndLines();

        bool isEof();

        bool isMainStructure();
    };
}


#endif //JS_AI_PARSER_H
