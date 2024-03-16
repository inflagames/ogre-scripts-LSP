#ifndef OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H
#define OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H

#include <iostream>
#include <string>
#include <map>
#include <set>

#include "lang/parser.h"
#include "lsp_protocol.h"
#include "lsp/formatter.h"
#include "lsp/symbols.h"
#include "lsp/semantic_tokens.h"

#define HEADER_CONTENT_LENGTH "Content-Length"
#define HEADER_CONTENT_TYPE "Content-Type"

namespace OgreScriptLSP {
    class LspServer {
    private:
        char ch = ' ';
        std::string message;
        ClientCapabilities clientCapabilities;

    public:
        std::map<std::string, std::unique_ptr<OgreScriptLSP::Parser>> parsers;
        bool running = false;

        LspServer() = default;

        void runServer(std::ostream &oos = std::cout, std::istream &ios = std::cin);

        void sendResponse(const std::string &msg, std::ostream &oos = std::cout);

        Action readHeaders(std::istream &os = std::cin);

        std::string readHeaderName(std::istream &os = std::cin);

        std::string readHeaderValue(std::istream &os = std::cin);

        void readContent(Action &action, std::istream &os = std::cin);

        void initialize(RequestMessage *rm, std::ostream &oos = std::cout);

        void formatting(RequestMessage *rm, bool withRange, std::ostream &oos = std::cout);

        void goToDefinition(RequestMessage *rm, std::ostream &oos = std::cout);

        void semanticTokens(RequestMessage *rm, std::ostream &oos);

        void documentSymbols(RequestMessage *rm, std::ostream &oos);

        void didOpen(RequestMessage *rm, std::ostream &oos);

        void didClose(RequestMessage *rm);

        void didChange(RequestMessage *rm, std::ostream &oos);

        void sendDiagnostic(Parser *parser, std::ostream &oos);

        OgreScriptLSP::Parser *getParserByUri(const std::string &uri);

        void updateParserByUri(const std::string &uri, OgreScriptLSP::Parser *parser);

        static ResponseMessage newResponseMessage(std::string id, ResultBase *result);

        static NotificationMessage newNotificationMessage(std::string method, ResultBase *params);

        void shutdown();

        void exit();

        char nextCharacter(std::istream &os = std::cin);

        OgreScriptLSP::Parser *updateParser(const std::string &uri, const std::string &code);
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H
