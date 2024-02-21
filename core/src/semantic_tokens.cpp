#include "../inc/semantic_tokens.h"

OgreScriptLSP::ResultBase *OgreScriptLSP::SemanticToken::getSemanticTokens(OgreScriptLSP::Parser *parser) {
    auto *res = new SemanticTokens();

    for (const auto &m: parser->getScript()->materials) {
        res->addToken(m->materialSymbol.line, m->materialSymbol.column, m->materialSymbol.size, 0, 0);
    }

    return res;
}
