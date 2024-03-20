#ifndef OGRE_SCRIPTS_LSP_LIB_SYMBOLS_H
#define OGRE_SCRIPTS_LSP_LIB_SYMBOLS_H


#include "../lsp_protocol.h"
#include "../lang/parser.h"

namespace OgreScriptLSP {
    class Symbols {
    public:
        static ResultArray *getSymbols(Parser* parser);
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_SYMBOLS_H
