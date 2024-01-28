#include <utility>

//
// Created by gonzalezext on 28.01.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
#define OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H

namespace OgreScriptLSP {
    struct BaseException : std::exception {
    public:
        std::string message;
        int line;
        int column;

        BaseException(std::string message, int line, int column)
                : message(std::move(message)), line(line), column(column) {}
    };

    struct ParseException : BaseException {
    public:
        explicit ParseException(std::string message, int line, int column)
                : BaseException(std::move(message), line, column) {}
    };
}

#endif //OGRE_SCRIPTS_LSP_LIB_EXCEPTIONS_H
