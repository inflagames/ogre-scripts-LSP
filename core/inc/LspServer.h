//
// Created by gonzalezext on 25.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LSPSERVER_H
#define OGRE_SCRIPTS_LSP_LIB_LSPSERVER_H

#include <iostream>
#include <string>

#include "LspProtocol.h"

#define HEADER_CONTENT_LENGTH "Content-Length"
#define HEADER_CONTENT_TYPE "Content-Type"

class LspServer {
private:
    bool running = false;
    char ch;

public:
    LspServer() = default;

    void runServer(std::ostream &oos = std::cout, std::istream &ios = std::cin);

    void sendResponse(std::string msj, std::ostream &oos = std::cout);

    Action readHeaders(std::istream &os = std::cin);

    std::string readHeaderName(std::istream &os = std::cin);

    std::string readHeaderValue(std::istream &os = std::cin);

    Action readContent(Action action, std::istream &os = std::cin);

    void shutdown();

    void exit();

    char nextCharacter(std::istream &os = std::cin);
};


#endif //OGRE_SCRIPTS_LSP_LIB_LSPSERVER_H
