#include "gtest/gtest.h"
#include "nlohmann/json.hpp"

#include "../src/lsp_server.h"
#include "utils/utils.h"

using namespace OgreScriptLSP;

TEST (LSPFormattingTest, documentSymbols_ShouldGetAllTheDocumentSymbols) {
    // toDo (gonzalezext)[21.02.24]: finish this test
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // request document symbols
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 9875, "method": "textDocument/documentSymbol", "params": {"textDocument": {"uri": "file://./examples/full_test_material.material"}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").is_array());
}
