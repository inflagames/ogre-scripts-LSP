//
// Created by gonzalezext on 08.02.24.
//

#ifndef OGRE_SCRIPTS_LSP_LIB_FORMATTER_H
#define OGRE_SCRIPTS_LSP_LIB_FORMATTER_H


#include "lsp_protocol.h"
#include "parser.h"

namespace OgreScriptLSP {
    class Formatter {
    public:
        static ResultArray *formatting(Parser *parser, FormattingOptions options, Range range = {{0, 0},
                                                                          {INT32_MAX, INT32_MAX}});

        static std::string repeatCharacter(char c, int times);
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_FORMATTER_H
