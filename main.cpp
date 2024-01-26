// start LSP

#include "core/inc/LspServer.h"

int main() {

    auto *lcp = new LspServer();
    lcp->runServer();

    return 0;
}