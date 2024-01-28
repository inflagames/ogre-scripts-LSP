// start LSP

#include "core/inc/lsp_server.h"

int main() {

    auto *lcp = new lsp_server();
    lcp->runServer();

    return 0;
}