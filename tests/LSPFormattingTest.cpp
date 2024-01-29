//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"
#include <nlohmann/json.hpp>

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

TEST (LSPFormattingTest, formattingFile_ShouldReturnTheEditListToFormatTheFile) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = "Content-Length: 167\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}";
    // initialized notification
    inputData += "Content-Length: 68\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 456, \"method\": \"initialized\", \"params\": {}}";
    // formatting request
    inputData += "Content-Length: 205\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/formatting\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/formatting_programs.material\"},\"options\": {\"tabSize\": 2, \"insertSpaces\": true}}}";
    // exit request
    inputData += "Content-Length: 58\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 7345, \"method\": \"exit\"}";

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;
//    std::cout << "----------------" << std::endl;
//    std::cout << outMock.str() << std::endl;
//    std::cout << "----------------" << std::endl;

//    ASSERT_TRUE(j.contains("id"));
//    ASSERT_EQ("1234", j.at("id").template get<std::string>());
//
//    ASSERT_TRUE(j.contains("result"));
//    ASSERT_TRUE(j.at("result").contains("capabilities"));
//    ASSERT_TRUE(j.at("result").at("capabilities").contains("documentFormattingProvider"));
//    ASSERT_TRUE(j.at("result").at("capabilities").at("documentFormattingProvider").template get<bool>());
//
//    ASSERT_TRUE(j.contains("result"));
//    ASSERT_TRUE(j.at("result").contains("serverInfo"));
//    ASSERT_TRUE(j.at("result").at("serverInfo").contains("name"));
//    ASSERT_EQ("ogre-scripts-LSP", j.at("result").at("serverInfo").at("name").template get<std::string>());
//    ASSERT_TRUE(j.at("result").at("serverInfo").contains("version"));
//    ASSERT_EQ("1.0.0", j.at("result").at("serverInfo").at("version").template get<std::string>());
}
