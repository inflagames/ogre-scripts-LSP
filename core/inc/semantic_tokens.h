#ifndef OGRE_SCRIPTS_LSP_LIB_SEMANTIC_TOKENS_H
#define OGRE_SCRIPTS_LSP_LIB_SEMANTIC_TOKENS_H


#include "lsp_protocol.h"
#include "parser.h"

namespace OgreScriptLSP {
    class SemanticToken {
    public:
        static ResultBase *getSemanticTokens(Parser *parser, Range range = {{0, 0},
                                                                            {INT32_MAX, INT32_MAX}});
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_SEMANTIC_TOKENS_H
