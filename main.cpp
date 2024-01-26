// start LSP

#include "core/inc/LspServer.h"

int main() {

    auto *lcp = new LspServer();

    while(true) {

        lcp->readHeaders();


//        std::cout << a << std::endl;
    }

    return 0;
}