// start LSP

#include "core/inc/lsp_server.h"

// toDo (gonzalezext)[08.02.24]: include some argument to configure the server
int main() {

    Logs::getInstance().enableLogs();

    auto *lcp = new OgreScriptLSP::LspServer();
    lcp->runServer();

    return 0;
}
