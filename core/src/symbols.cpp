#include "../inc/symbols.h"

OgreScriptLSP::ResultArray *OgreScriptLSP::Symbols::getSymbols(OgreScriptLSP::Parser *parser) {
    auto *res = new ResultArray();

    for (const auto &m: parser->getScript()->materials) {
        res->elements.push_back(new DocumentSymbol(m->materialSymbol.literal, Class,
                                                   m->materialSymbol.toRange(), m->materialSymbol.toRange()));
    }

    return res;
}
