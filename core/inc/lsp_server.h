//
// Created by gonzalezext on 25.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H
#define OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H

#include <iostream>
#include <string>
#include <map>
#include <set>

#include "lsp_protocol.h"
#include "parser.h"

#define HEADER_CONTENT_LENGTH "Content-Length"
#define HEADER_CONTENT_TYPE "Content-Type"

class LspServer {
private:
    bool running = false;
    char ch = ' ';
    std::string message;

    std::map<std::string, OgreScriptLSP::Parser *> parsers;
    std::set<std::string> parsersMarkedAsUpdated;

public:
    LspServer() = default;

    void runServer(std::ostream &oos = std::cout, std::istream &ios = std::cin);

    void sendResponse(std::string msg, std::ostream &oos = std::cout);

    Action readHeaders(std::istream &os = std::cin);

    std::string readHeaderName(std::istream &os = std::cin);

    std::string readHeaderValue(std::istream &os = std::cin);

    Action readContent(Action action, std::istream &os = std::cin);

    void formatting(RequestMessage *rm, bool withRange, std::ostream &oos = std::cout);

    void goToDefinition(RequestMessage *rm, std::ostream &oos = std::cout);

    void didOpen(RequestMessage *rm);

    void didClose(RequestMessage *rm);

    void didChange(RequestMessage *rm);

    OgreScriptLSP::Parser *getParserByUri(const std::string &uri);

    void shutdown();

    void exit();

    char nextCharacter(std::istream &os = std::cin);
};


#endif //OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H
