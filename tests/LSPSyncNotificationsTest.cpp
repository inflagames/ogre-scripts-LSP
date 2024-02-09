#include <thread>

#include "gtest/gtest.h"

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"


using namespace OgreScriptLSP;

// start server in thread
void runServer(LspServer *lsp, std::ostream &oos, std::istream &ios) {
    lsp->runServer(oos, ios);
}

void waitForSize(LspServer *lsp, int sizeValue) {
    uint64_t start = test_utils::getTimeNow();
    while (lsp->parsers.size() != sizeValue) {
        uint64_t current = test_utils::getTimeNow();
        if (current - start > TIMEOUT_MS) {
            std::cout << "Timeout waiting for size to be equals: " << sizeValue << ". Current value: "
                      << lsp->parsers.size() << std::endl;
            ASSERT_TRUE(false);
        }
    }
}


TEST (LSPSyncTest, clientSync_LspServerShouldOpenCloseChangeFiles_validateLspAfterEachSync) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/formatting", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"},"options": {"tabSize": 2, "insertSpaces": true}}})");

    // system in/out mocks
    auto *stringstreambuf = new test_utils::my_stringstreambuf(inputData);
    std::istream inMock(stringstreambuf);
    std::stringstream outMock;

    // run server on thread
    std::thread runServerThread(runServer, std::ref(lsp), std::ref(outMock), std::ref(inMock));

    // with the formatting, the parser map should be updated
    waitForSize(lsp, 1);

    // send close file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didClose", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 0);

    // open 2 files
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 1);
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 1);
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_tab_size_4_spaces.material"}}})"));
    waitForSize(lsp, 2);

    // send close file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didClose", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_tab_size_4_spaces.material"}}})"));
    waitForSize(lsp, 1);

    // exit request
    stringstreambuf->appendStr(test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})"));
    stringstreambuf->setEof();

    // wait for thread
    runServerThread.join();
}
