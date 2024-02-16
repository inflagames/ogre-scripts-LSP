// start LSP

#include "core/inc/lsp_server.h"
#include "core/inc/program_arg.h"

int main(int argc, const char **argv) {
    {
        // handle application arguments
        std::unique_ptr<ProgramArg> programArg = std::make_unique<ProgramArg>(argc, argv);
        if (programArg->shouldShowHelp()) {
            programArg->showHelp();
            return 0;
        }

        if (programArg->shouldLogging()) {
            Logs::getInstance().enableLogs();
        }
    }

    auto *lcp = new OgreScriptLSP::LspServer();
    lcp->runServer();

    return 0;
}
