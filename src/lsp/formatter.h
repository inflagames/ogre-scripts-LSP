#ifndef OGRE_SCRIPTS_LSP_LIB_FORMATTER_H
#define OGRE_SCRIPTS_LSP_LIB_FORMATTER_H


#include "../lsp_protocol.h"
#include "../lang/parser.h"

namespace OgreScriptLSP {
    class Formatter {
    public:
        static ResultArray *formatting(Parser *parser, FormattingOptions options, Range range = {{0, 0},
                                                                          {INT32_MAX, INT32_MAX}});

        static std::string repeatCharacter(char c, std::size_t times);

        static std::optional<Range> inException(Range range, Parser *parser);

        static int tokenSpacing(Token tk);
    };
}


#endif //OGRE_SCRIPTS_LSP_LIB_FORMATTER_H
