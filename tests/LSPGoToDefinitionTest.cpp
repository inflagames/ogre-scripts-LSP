#include "gtest/gtest.h"
#include <nlohmann/json.hpp>

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

using namespace OgreScriptLSP;

/**
 * @brief Asserts that a given range matches the expected range specified in a JSON object.
 *
 * This function compares a given range (specified as a pair of pairs representing start and end positions)
 * with the range values extracted from a JSON object. It uses assertions to verify that the range values
 * extracted from the JSON object match the expected values.
 *
 * @param r A pair of pairs representing the start and end positions of the range to be asserted.
 * @param j The JSON object containing the expected range values under keys "start" and "end".
 *
 * @return void
 *
 * @remarks
 *  - The function expects the JSON object to have keys "start" and "end", each containing objects with keys
 *    "line" and "character" representing the line and character positions respectively.
 *  - It utilizes assertions to compare the range values extracted from the JSON object with the expected range
 *    values provided in the input range parameter.
 *  - If any of the assertions fail, it indicates a mismatch between the expected and actual range values,
 *    which may signify an inconsistency in the data being processed.
 *
 * @dependencies
 *  - The function relies on the availability and correct implementation of the ASSERT_EQ macro, which is typically
 *    provided by testing frameworks such as Google Test or similar testing libraries.
 *  - It assumes that the input JSON object (of type nlohmann::json) contains valid data conforming to the expected
 *    structure described above.
 *
 * @note
 *  - Developers should ensure that the input range and JSON object are correctly formatted and represent valid
 *    range data before invoking this function.
 *  - This function is particularly useful in testing scenarios where verification of range values extracted from
 *    JSON objects is necessary to ensure correctness of data processing algorithms or functionalities.
 *  - It is recommended to handle potential exceptions thrown by accessing JSON values or by the assertion macros
 *    to provide better error handling and reporting capabilities.
 */
void assertRange(range r, nlohmann::json j) {
    ASSERT_EQ(r.first.first, j.at("start").at("line").template get<int>());
    ASSERT_EQ(r.first.second, j.at("start").at("character").template get<int>());
    ASSERT_EQ(r.second.first, j.at("end").at("line").template get<int>());
    ASSERT_EQ(r.second.second, j.at("end").at("character").template get<int>());
}

/**
 * @brief Validates the response of a "go to" action against the expected range and file URI.
 *
 * This function performs validation on the response obtained from a "go to" action triggered by clicking at a specific
 * position within a file. It sends requests to a Language Server Protocol (LSP) server instance, simulating the
 * initialization, a "go to" request, and then validates the response against the expected range and file URI.
 *
 * @param clickPosition A pair representing the line and character positions of the click position within the file.
 * @param expectedRange The expected range to which the "go to" action should navigate within the file.
 * @param fileUri The URI of the file against which the "go to" action is performed.
 *
 * @return void
 *
 * @remarks
 *  - This function creates a new instance of the LspServer class, simulating the behavior of an LSP server.
 *  - It constructs and sends JSON-RPC messages to the server to simulate the "initialize", "textDocument/definition",
 *    and "exit" requests necessary for the "go to" action.
 *  - The function utilizes input/output stream mocks to handle communication with the simulated LSP server, allowing
 *    for controlled testing of response validation.
 *  - It extracts and parses the response JSON object obtained from the server and validates its structure and content
 *    against the expected range and file URI.
 *
 * @dependencies
 *  - This function relies on the LspServer class for simulating the behavior of an LSP server.
 *  - It also depends on the test_utils namespace for generating JSON-RPC messages, extracting JSON responses, and
 *    performing assertion checks.
 *  - Proper initialization and configuration of the LSP server instance are assumed for accurate testing results.
 *
 * @note
 *  - Developers should ensure that the input parameters (click position, expected range, and file URI) accurately
 *    represent the scenario being tested.
 *  - This function is particularly useful in automated testing scenarios to verify the correctness of "go to" actions
 *    implemented within an LSP server.
 *  - It is recommended to handle potential exceptions or errors that may occur during JSON parsing or response
 *    validation to provide robust error handling mechanisms.
 */
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
    validateGoTo({66, 44}, {{0, 17},
                            {0, 36}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_sameMaterialDefinition) {
    validateGoTo({59, 40}, {{10, 15},
                            {10, 34}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_geometryExample) {
    validateGoTo({70, 40}, {{14, 17},
                            {14, 36}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_hullExample) {
    validateGoTo({74, 60}, {{18, 26},
                            {18, 77}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_domainExample) {
    validateGoTo({77, 55}, {{30, 28},
                            {30, 79}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_computedProgramExample) {
    validateGoTo({80, 50}, {{45, 16},
                            {45, 68}}, "file://./examples/lsp/goto_definition_program.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_techniqueDefinitionPosition) {
    validateGoTo({0, 20}, {{0, 19},
                           {0, 35}}, "file://./examples/lsp/goto_definition_technique.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_passRtshaderSystemDefinitionPosition) {
    validateGoTo({18, 33}, {{0, 25},
                            {0, 34}}, "file://./examples/lsp/goto_definition_rtshader.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_textureUnitRtshaderSystemDefinitionPosition) {
    validateGoTo({13, 30}, {{0, 25},
                            {0, 34}}, "file://./examples/lsp/goto_definition_rtshader.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_shadowReceiverExample) {
    validateGoTo({8, 40}, {{0, 9},
                            {0, 32}}, "file://./examples/lsp/goto_definition_shadow_mat.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_shadowCasterExample) {
    validateGoTo({9, 44}, {{3, 9},
                            {3, 30}}, "file://./examples/lsp/goto_definition_shadow_mat.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_sharedParamsExample) {
    validateGoTo({17, 40}, {{6, 14},
                           {6, 26}}, "file://./examples/lsp/goto_definition_shared_param.material");
}

TEST (LSPGoToDefinitionTest, definitionProgram_ShouldReturnGoToDefinition_sharedParamsInObjectExample) {
    validateGoTo({6, 20}, {{6, 14},
                            {6, 26}}, "file://./examples/lsp/goto_definition_shared_param.material");
}
