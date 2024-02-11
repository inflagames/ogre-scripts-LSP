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

/**
 * This test is replicating a bug from the integration with JetBrains plugin
 */
TEST (LSPSyncTest, clientSync_CoverServerCrashIssueInServer_validateBug) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})");

    // system in/out mocks
    auto *stringstreambuf = new test_utils::my_stringstreambuf(inputData);
    std::istream inMock(stringstreambuf);
    std::stringstream outMock;

    // run server on thread
    std::thread runServerThread(runServer, std::ref(lsp), std::ref(outMock), std::ref(inMock));

    // with the formatting, the parser map should be updated
    waitForSize(lsp, 1);

    // send updated file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}, "contentChanges": [{"text": "fragment_program Game/Floor/ShaderFp glsl glsles {\n    source FloorFp.glsl\n}\n\nmaterial something {\n    technique {\n        pass {\n        }\n    }\n}\n\n\nmaterial kk : something {\n\n}\n\nvertex_program Game/Floor/ShaderVp glsl {\n    source FloorVp.glsl\n\n    default_params {\n        // assign samplers as required by GLSL\n        param_named normalMap int 0\n    }\n}\n"}]}})"
    ));
    while (stringstreambuf->currentChar() != WAITING_DATA);
    ASSERT_TRUE(lsp->running);

    // send updated file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}, "contentChanges": [{"text": "fragment_program Game/Floor/ShaderFp glsl glsles {\n    source FloorFp.glsl\n}\n\nmaterial something {\n    technique {\n        pass {\n        }\n    }\n}\n\n\nmaterial kk : something {j\n\n}\n\nvertex_program Game/Floor/ShaderVp glsl {\n    source FloorVp.glsl\n\n    default_params {\n        // assign samplers as required by GLSL\n        param_named normalMap int 0\n    }\n}\n"}]}})"
    ));
    while (stringstreambuf->currentChar() != WAITING_DATA);
    sleep(1);// sleep for a second
    ASSERT_TRUE(lsp->running);// at this point the server crash

    // exit request
    stringstreambuf->appendStr(test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})"));
    stringstreambuf->setEof();

    // wait for thread
    runServerThread.join();
}
