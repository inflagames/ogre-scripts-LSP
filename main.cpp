// start LSP

#include "core/inc/lsp_server.h"

int main() {

    auto *lcp = new LspServer();
    lcp->runServer();

    return 0;
}