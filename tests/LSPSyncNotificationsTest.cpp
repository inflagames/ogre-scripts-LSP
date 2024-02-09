#include <thread>
#include <chrono>

#include "gtest/gtest.h"

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

using namespace OgreScriptLSP;

// start server in thread
void runServer(LspServer *lsp, std::ostream &oos, std::istream &ios) {
    lsp->runServer(oos, ios);
}

int parserSize(LspServer *lsp) {
    return (int) lsp->parsers.size();
}

int parserMarkedSize(LspServer *lsp) {
    return (int) lsp->parsersMarkedAsUpdated.size();
}

void waitForSize(LspServer *lsp, int sizeValue, int (*sizeFun)(LspServer *)) {
    uint64_t timeout = 2000; // 2 seconds
    uint64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    while (sizeFun(lsp) != sizeValue) {
        uint64_t current = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        if (current - start > timeout) {
            std::cout << "Timeout waiting for size to be equals: " << sizeValue << ". Current value: "
                      << sizeFun(lsp) << std::endl;
            ASSERT_TRUE(false);
        }
    }
}


TEST (LSPSyncTest, clientSync_LspServerShouldOpenCloseAfterTheNotifications) {
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
    auto *b = new test_utils::MyBuf(inputData);
    std::istream inMock(b);
    std::stringstream outMock;

    // run server on thread
    std::thread runServerThread(runServer, std::ref(lsp), std::ref(outMock), std::ref(inMock));

    // with the formatting, the parser map should be updated
    waitForSize(lsp, 1, &parserSize);

    // send close file
    b->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didClose", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 0, &parserSize);

    // open 2 files
    b->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 1, &parserSize);
    b->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 1, &parserSize);
    b->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_tab_size_4_spaces.material"}}})"));
    waitForSize(lsp, 2, &parserSize);

    b->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_tab_size_4_spaces.material"}}})"));
    waitForSize(lsp, 1, &parserMarkedSize);

    // send close file
    b->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didClose", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_tab_size_4_spaces.material"}}})"));
    waitForSize(lsp, 1, &parserSize);
    waitForSize(lsp, 0, &parserMarkedSize);

    // exit request
    b->appendStr(test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})"));
    b->setEof();

    // wait for thread
    runServerThread.join();
}
