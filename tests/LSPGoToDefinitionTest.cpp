#include "gtest/gtest.h"
#include <nlohmann/json.hpp>

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

TEST (LSPGoToDefinitionTest, definitionMaterial_ShouldReturnGoToDefinition_validaDefinition) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = "Content-Length: 167\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}";
    // initialized notification
    inputData += "Content-Length: 68\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 456, \"method\": \"initialized\", \"params\": {}}";
    // formatting request
    inputData += "Content-Length: 204\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/definition\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/goto_definition_material.material\"},\"position\": {\"line\": 25, \"character\": 20}}}";
    // exit request
    inputData += "Content-Length: 58\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 7345, \"method\": \"exit\"}";

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
//    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_EQ("500", j.at("id").template get<std::string>());

    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(!j.at("result").is_null());

    // validate uri
    ASSERT_TRUE(j.at("result").contains("uri"));
    ASSERT_EQ("file://./examples/lsp/goto_definition_material.material",
              j.at("result").at("uri").template get<std::string>());

    // validate range
    ASSERT_TRUE(j.at("result").contains("range"));
    ASSERT_EQ(0, j.at("result").at("range").at("start").at("line").template get<int>());
    ASSERT_EQ(9, j.at("result").at("range").at("start").at("character").template get<int>());
    ASSERT_EQ(0, j.at("result").at("range").at("end").at("line").template get<int>());
    ASSERT_EQ(14, j.at("result").at("range").at("end").at("character").template get<int>());
}

TEST (LSPGoToDefinitionTest, definitionMaterial_ShouldReturnGoToDefinition_sameMaterialDefinition) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = "Content-Length: 167\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}";
    // initialized notification
    inputData += "Content-Length: 68\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 456, \"method\": \"initialized\", \"params\": {}}";
    // formatting request
    inputData += "Content-Length: 204\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/definition\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/goto_definition_material.material\"},\"position\": {\"line\": 25, \"character\": 13}}}";
    // exit request
    inputData += "Content-Length: 58\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 7345, \"method\": \"exit\"}";

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_EQ("500", j.at("id").template get<std::string>());

    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(!j.at("result").is_null());

    // validate uri
    ASSERT_TRUE(j.at("result").contains("uri"));
    ASSERT_EQ("file://./examples/lsp/goto_definition_material.material",
              j.at("result").at("uri").template get<std::string>());

    // validate range
    ASSERT_TRUE(j.at("result").contains("range"));
    ASSERT_EQ(25, j.at("result").at("range").at("start").at("line").template get<int>());
    ASSERT_EQ(9, j.at("result").at("range").at("start").at("character").template get<int>());
    ASSERT_EQ(25, j.at("result").at("range").at("end").at("line").template get<int>());
    ASSERT_EQ(13, j.at("result").at("range").at("end").at("character").template get<int>());
}

TEST (LSPGoToDefinitionTest, definitionMaterial_ShouldReturnGoToDefinition_byDefaultSamePosition) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = "Content-Length: 167\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}";
    // initialized notification
    inputData += "Content-Length: 68\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 456, \"method\": \"initialized\", \"params\": {}}";
    // formatting request
    inputData += "Content-Length: 204\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/definition\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/goto_definition_material.material\"},\"position\": {\"line\": 13, \"character\": 24}}}";
    // exit request
    inputData += "Content-Length: 58\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 7345, \"method\": \"exit\"}";

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_EQ("500", j.at("id").template get<std::string>());

    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(!j.at("result").is_null());

    // validate uri
    ASSERT_TRUE(j.at("result").contains("uri"));
    ASSERT_EQ("file://./examples/lsp/goto_definition_material.material",
              j.at("result").at("uri").template get<std::string>());

    // validate range
    ASSERT_TRUE(j.at("result").contains("range"));
    ASSERT_EQ(13, j.at("result").at("range").at("start").at("line").template get<int>());
    ASSERT_EQ(24, j.at("result").at("range").at("start").at("character").template get<int>());
    ASSERT_EQ(13, j.at("result").at("range").at("end").at("line").template get<int>());
    ASSERT_EQ(24, j.at("result").at("range").at("end").at("character").template get<int>());
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_validaDefinition) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = "Content-Length: 167\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}";
    // initialized notification
    inputData += "Content-Length: 68\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 456, \"method\": \"initialized\", \"params\": {}}";
    // formatting request
    inputData += "Content-Length: 203\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/definition\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/goto_definition_program.material\"},\"position\": {\"line\": 27, \"character\": 44}}}";
    // exit request
    inputData += "Content-Length: 58\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 7345, \"method\": \"exit\"}";

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
//    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_EQ("500", j.at("id").template get<std::string>());

    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(!j.at("result").is_null());

    // validate uri
    ASSERT_TRUE(j.at("result").contains("uri"));
    ASSERT_EQ("file://./examples/lsp/goto_definition_program.material",
              j.at("result").at("uri").template get<std::string>());

    // validate range
    ASSERT_TRUE(j.at("result").contains("range"));
    ASSERT_EQ(0, j.at("result").at("range").at("start").at("line").template get<int>());
    ASSERT_EQ(17, j.at("result").at("range").at("start").at("character").template get<int>());
    ASSERT_EQ(0, j.at("result").at("range").at("end").at("line").template get<int>());
    ASSERT_EQ(35, j.at("result").at("range").at("end").at("character").template get<int>());
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_sameMaterialDefinition) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = "Content-Length: 167\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 1234, \"method\": \"initialize\", \"params\": {\"processId\": 31, \"clientInfo\": {\"name\": \"client-name\"}, \"rootUri\": \"/some/that\", \"capabilities\": {}}}";
    // initialized notification
    inputData += "Content-Length: 68\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 456, \"method\": \"initialized\", \"params\": {}}";
    // formatting request
    inputData += "Content-Length: 203\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/definition\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/goto_definition_program.material\"},\"position\": {\"line\": 10, \"character\": 29}}}";
    // exit request
    inputData += "Content-Length: 58\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\": \"2.0\", \"id\": 7345, \"method\": \"exit\"}";

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_EQ("500", j.at("id").template get<std::string>());

    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(!j.at("result").is_null());

    // validate uri
    ASSERT_TRUE(j.at("result").contains("uri"));
    ASSERT_EQ("file://./examples/lsp/goto_definition_program.material",
              j.at("result").at("uri").template get<std::string>());

    // validate range
    ASSERT_TRUE(j.at("result").contains("range"));
    ASSERT_EQ(10, j.at("result").at("range").at("start").at("line").template get<int>());
    ASSERT_EQ(15, j.at("result").at("range").at("start").at("character").template get<int>());
    ASSERT_EQ(10, j.at("result").at("range").at("end").at("line").template get<int>());
    ASSERT_EQ(33, j.at("result").at("range").at("end").at("character").template get<int>());
}
