#ifndef JS_SCRIPT_AI_LIBRARY_H
#define JS_SCRIPT_AI_LIBRARY_H

#include <fstream>
#include <vector>
#include <string>

#include "token.h"
#include "exceptions.h"

namespace OgreScriptLSP {
    class Scanner {
    public:
        std::ifstream file;
        char ch{};
        int lineCount = 0;
        int columnCount = 0;
        bool isNewLine = false;

        std::vector<BaseException> exceptions;

        Scanner();

        /**
         * Return the list of token extracted from the script file
         * @return
         */
        std::vector<TokenValue> parse();

        /**
         * Read and return the next token from script file
         * @return
         */
        TokenValue nextToken();

        void consumeComment(bool lineComment = true);

        TokenValue consumeString(char stringDelimiter);

        TokenValue consumeMatch();

        TokenValue symbolToken(Token tk);

        TokenValue consumeNumber(bool isFirstPeriod = true);

        TokenValue nextLiteral();

        static bool validLiteral(char c, bool startCharacter = true);

        void recuperateError(const std::string& error = "");

        /**
         * Consume next character from the script file
         * @return false if file reach isEof or true otherwise
         */
        bool nextCharacter();

        /**
         * Consume all the empty and end-lines in a row until it reach a new character
         * @return
         */
        bool consumeEmpty();

        void loadScript(const std::string &scriptFile);
    };
}

#endif //JS_SCRIPT_AI_LIBRARY_H
