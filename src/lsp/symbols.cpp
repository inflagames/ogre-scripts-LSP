#include "symbols.h"

OgreScriptLSP::ResultArray *OgreScriptLSP::Symbols::getSymbols(OgreScriptLSP::Parser *parser) {
    auto *res = new ResultArray();

    for (const auto &m: parser->getScript()->materials) {
        res->elements.emplace_back(new DocumentSymbol(m->symbol.literal, Class,
                                                   m->symbol.toRange(), m->symbol.toRange()));
    }

    return res;
}
