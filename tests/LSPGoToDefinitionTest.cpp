#include "gtest/gtest.h"
#include <nlohmann/json.hpp>

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

using namespace OgreScriptLSP;

void assertRange(range r, nlohmann::json j) {
    ASSERT_EQ(r.first.first, j.at("start").at("line").template get<int>());
    ASSERT_EQ(r.first.second, j.at("start").at("character").template get<int>());
    ASSERT_EQ(r.second.first, j.at("end").at("line").template get<int>());
    ASSERT_EQ(r.second.second, j.at("end").at("character").template get<int>());
}

void validateGoTo(position clickPosition, range expectedRange, const std::string &fileUri) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "file://./examples", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/definition", "params": {"textDocument": {"uri": ")" +
            fileUri + R"("},"position": {"line": )" +
            std::to_string(clickPosition.first) + ", \"character\": " + std::to_string(clickPosition.second) + "}}}");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

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
    ASSERT_EQ(fileUri, j.at("result").at("uri").template get<std::string>());

    // validate range
    ASSERT_TRUE(j.at("result").contains("range"));
    assertRange(expectedRange, j.at("result").at("range"));
}

TEST (LSPGoToDefinitionTest, definitionMaterial_ShouldReturnGoToDefinition_validaDefinition) {
    validateGoTo({25, 20}, {{0, 9},
                            {0, 15}}, "file://./examples/lsp/goto_definition_material.material");
}

TEST (LSPGoToDefinitionTest, definitionMaterial_ShouldReturnGoToDefinition_sameMaterialDefinition) {
    validateGoTo({25, 13}, {{25, 9},
                            {25, 14}}, "file://./examples/lsp/goto_definition_material.material");
}

TEST (LSPGoToDefinitionTest, definitionMaterial_ShouldReturnGoToDefinition_byDefaultSamePosition) {
    validateGoTo({13, 24}, {{13, 24},
                            {13, 24}}, "file://./examples/lsp/goto_definition_material.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_validaDefinition) {
    validateGoTo({27, 44}, {{0, 17},
                            {0, 36}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_sameMaterialDefinition) {
    validateGoTo({10, 29}, {{10, 15},
                            {10, 34}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_techniqueDefinitionPosition) {
    validateGoTo({0, 20}, {{0, 19},
                            {0, 35}}, "file://./examples/lsp/goto_definition_technique.material");
}
