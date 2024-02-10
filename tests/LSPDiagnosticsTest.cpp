#include "gtest/gtest.h"

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"


TEST (LSPDiagnostigsTest, diagnostigFile_ShouldSendANotificationWithTheDiagnostigs) {
    auto *lsp = new OgreScriptLSP::LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/parser/frag_vert_declarations_with_errors.material"}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    ASSERT_EQ(6, j.at("params").at("diagnostics").size());
}

TEST (LSPDiagnostigsTest, diagnostigFile_ShouldSendANotificationWithTheDiagnostigsAfterDidChange) {
    auto *lsp = new OgreScriptLSP::LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // didOpen request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/parser/frag_vert_declarations_with_errors.material"}}})");
    // didChange request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 501, "method": "textDocument/didChange", "params": {"contentChanges": [{"text": "fragment_program Game/Floor/ShaderFp glsl glsles {\n  0 source FloorFp.glsl\n}\n"}], "textDocument": {"uri": "file://./examples/parser/frag_vert_declarations_with_errors.material"}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");
    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 3));
    ASSERT_EQ(1, j.at("params").at("diagnostics").size());
}
