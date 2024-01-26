//
// Created by gonzalezext on 25.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_LSPSERVER_H
#define OGRE_SCRIPTS_LSP_LIB_LSPSERVER_H

#include <iostream>
#include <string>

#include "LspProtocol.h"

class LspServer {
private:
    char ch;

public:
    LspServer() = default;

    Action readHeaders(std::istream &os = std::cin);

    std::string readHeaderName(std::istream &os = std::cin);

    std::string readHeaderValue(std::istream &os = std::cin);

    Action readContent(Action action, std::istream &os = std::cin);

    char nextCharacter(std::istream &os = std::cin);
};


#endif //OGRE_SCRIPTS_LSP_LIB_LSPSERVER_H
