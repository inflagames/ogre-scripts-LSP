#include <utility>

//
// Created by gonzalezext on 28.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
#define OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H

namespace OgreScriptLSP {
    struct ParseException : std::exception {
    public:
        std::string message;

        explicit ParseException(std::string message) : message(std::move(message)) {}
    };
}

#endif //OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
