//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"
#include <nlohmann/json.hpp>

#include "../core/inc/LspServer.h"
#include "utils.h"

TEST (LSPInitializeTest, readHeadBody_ShouldReadCorrectlyTheHeadAndBody) {
    auto *lsp = new LspServer();
    std::istringstream osMock(
            "Content-Length: 193\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}");

    Action msg = lsp->readHeaders(osMock);

    ASSERT_EQ(193, msg.contentLength);
    ASSERT_EQ("application/vscode; charset=utf-8", msg.contentType);

    msg = lsp->readContent(msg, osMock);

    ASSERT_TRUE(msg.message != nullptr);
    RequestMessage *rm = ((RequestMessage *) msg.message);
    ASSERT_EQ("2.0", rm->jsonrpc);
    ASSERT_EQ("1234", rm->id);
    ASSERT_EQ("initialize", rm->method);

    InitializeParams *p = ((InitializeParams *) rm->params);
    ASSERT_EQ(31, p->processId);
    ASSERT_EQ("/some/that", p->rootUri);
}

TEST (LSPInitializeTest, readHeadBody_ShouldInitializeConnectionWithClient) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = "Content-Length: 167\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}";
    // initialized notification
    inputData += "Content-Length: 68\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 456, \"method\": \"initialized\", \"params\": {}}";
    // exit request
    inputData += "Content-Length: 58\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 7345, \"method\": \"exit\"}";

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str()));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_EQ("1234", j.at("id").template get<std::string>());

    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").contains("capabilities"));
    ASSERT_TRUE(j.at("result").at("capabilities").contains("documentFormattingProvider"));
    ASSERT_TRUE(j.at("result").at("capabilities").at("documentFormattingProvider").template get<bool>());

    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").contains("serverInfo"));
    ASSERT_TRUE(j.at("result").at("serverInfo").contains("name"));
    ASSERT_EQ("ogre-scripts-LSP", j.at("result").at("serverInfo").at("name").template get<std::string>());
    ASSERT_TRUE(j.at("result").at("serverInfo").contains("version"));
    ASSERT_EQ("1.0.0", j.at("result").at("serverInfo").at("version").template get<std::string>());
}
