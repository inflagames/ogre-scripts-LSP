//
// Created by gonzalezext on 25.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H
#define OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H

#include <iostream>
#include <string>

#include "lsp_protocol.h"
#include "parser.h"

#define HEADER_CONTENT_LENGTH "Content-Length"
#define HEADER_CONTENT_TYPE "Content-Type"

class lsp_server {
private:
    bool running = false;
    char ch;

public:
    lsp_server() = default;

    void runServer(std::ostream &oos = std::cout, std::istream &ios = std::cin);

    void sendResponse(std::string msj, std::ostream &oos = std::cout);

    Action readHeaders(std::istream &os = std::cin);

    std::string readHeaderName(std::istream &os = std::cin);

    std::string readHeaderValue(std::istream &os = std::cin);

    Action readContent(Action action, std::istream &os = std::cin);

    void formatting(DocumentFormattingParams *params, std::ostream &oos = std::cout);

    void shutdown();

    void exit();

    char nextCharacter(std::istream &os = std::cin);
};


#endif //OGRE_SCRIPTS_LSP_LIB_LSP_SERVER_H
