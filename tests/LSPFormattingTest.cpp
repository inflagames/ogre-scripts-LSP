#include "gtest/gtest.h"
#include "nlohmann/json.hpp"

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

using namespace OgreScriptLSP;

/**
 * @brief Validates the structure of an edit operation represented by a JSON object.
 *
 * This function validates the structure of an edit operation represented by a JSON object. It ensures that the JSON
 * object contains the necessary fields and subfields expected for an edit operation, including "newText" for the edited
 * content and "range" for specifying the range of text to be edited.
 *
 * @param it The JSON object representing the edit operation.
 *
 * @return void
 *
 * @remarks
 *  - This function performs structural validation of the JSON object to ensure that it conforms to the expected format
 *    for representing edit operations within the Language Server Protocol (LSP).
 *  - It checks for the presence of key fields ("newText" and "range") as well as subfields ("line" and "character")
 *    within the "start" and "end" objects under the "range" field.
 *
 * @dependencies
 *  - This function relies on the nlohmann::json type for representing JSON objects and the ASSERT_TRUE macro for
 *    performing assertion checks.
 *
 * @note
 *  - Developers should ensure that the input JSON object accurately represents the edit operation being validated.
 *  - This function is intended for use in testing scenarios to verify the correctness of edit operations generated
 *    by language processing tools or components.
 *  - It is recommended to handle potential exceptions or errors that may occur during JSON parsing or validation to
 *    provide robust error handling mechanisms.
 */
void validateEdit(nlohmann::json it) {
    ASSERT_TRUE(it.contains("newText"));
    ASSERT_TRUE(it.contains("range"));
    ASSERT_TRUE(it.at("range").contains("start"));
    ASSERT_TRUE(it.at("range").at("start").contains("line"));
    ASSERT_TRUE(it.at("range").at("start").contains("character"));
    ASSERT_TRUE(it.at("range").contains("end"));
    ASSERT_TRUE(it.at("range").at("end").contains("line"));
    ASSERT_TRUE(it.at("range").at("end").contains("character"));
}

std::vector<edit > extractRanges(nlohmann::json j) {
    std::vector<edit > edits;
    for (auto it: j) {
        validateEdit(it);

        range r = {
                {
                        it.at("range").at("start").at("line").template get<int>(),
                        it.at("range").at("start").at("character").template get<int>()
                },
                {
                        it.at("range").at("end").at("line").template get<int>(),
                        it.at("range").at("end").at("character").template get<int>()
                }};
        edits.emplace_back(r, it.at("newText").template get<std::string>());
    }
    return edits;
}

TEST (LSPFormattingTest, formattingFile_ShouldFormatTheFile_basicFormatting) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/formatting", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs.material"},"options": {"tabSize": 2, "insertSpaces": true}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

    // read file
    std::string initialCode = test_utils::readFile("./examples/lsp/formatting_programs.material");
    std::string resultCode = test_utils::readFile("./examples/lsp/formatting_programs_basic.material");

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").is_array());

    std::vector<edit > edits = extractRanges(j.at("result"));
    std::string resultText = test_utils::applyModifications(initialCode, edits);

//    std::cout << "----------------" << std::endl;
//    std::cout << resultText << std::endl;
//    std::cout << "----------------" << std::endl;

    ASSERT_EQ(resultCode, resultText);
}

TEST (LSPFormattingTest, formattingFile_ShouldFormatTheFile_withTabSizeOf4Spaces) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // open request before formatting
    std::string fileToOpen = test_utils::readFile("./examples/lsp/formatting_programs.material", true);
    inputData += test_utils::getMessageStr(
            "{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/didOpen\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/formatting_programs.material\", \"text\": \"" +
            fileToOpen + "\"}}}"
    );
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/formatting", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs.material"},"options": {"tabSize": 4, "insertSpaces": true}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

    // read file
    std::string initialCode = test_utils::readFile("./examples/lsp/formatting_programs.material");
    std::string resultCode = test_utils::readFile("./examples/lsp/formatting_programs_tab_size_4_spaces.material");

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 3));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").is_array());

    std::vector<edit > edits = extractRanges(j.at("result"));
    std::string resultText = test_utils::applyModifications(initialCode, edits);

//    std::cout << "----------------" << std::endl;
//    std::cout << resultText << std::endl;
//    std::cout << "----------------" << std::endl;

    ASSERT_EQ(resultCode, resultText);
}

TEST (LSPFormattingTest, formattingFile_ShouldFormatWithFinalNewLine) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/formatting", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs.material"},"options": {"tabSize": 2, "insertSpaces": false}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

    // read file
    std::string initialCode = test_utils::readFile("./examples/lsp/formatting_programs.material");
    std::string resultCode = test_utils::readFile("./examples/lsp/formatting_programs_with_tabs.material");

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").is_array());

    std::vector<edit > edits = extractRanges(j.at("result"));
    std::string resultText = test_utils::applyModifications(initialCode, edits);

//    std::cout << "----------------" << std::endl;
//    std::cout << resultText << std::endl;
//    std::cout << "----------------" << std::endl;

    ASSERT_EQ(resultCode, resultText);
}

TEST (LSPFormattingTest, formattingFile_ShouldFormatTheFile_withoutEndLine) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/formatting", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_without_endl.material"},"options": {"tabSize": 2, "insertSpaces": true, "insertFinalNewline": true}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

    // read file
    std::string initialCode = test_utils::readFile("./examples/lsp/formatting_programs_without_endl.material");
    std::string resultCode = test_utils::readFile("./examples/lsp/formatting_programs_with_endl.material");

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").is_array());

    std::vector<edit > edits = extractRanges(j.at("result"));
    std::string resultText = test_utils::applyModifications(initialCode, edits);

//    std::cout << "----------------" << std::endl;
//    std::cout << resultText << std::endl;
//    std::cout << "----------------" << std::endl;

    ASSERT_EQ(resultCode, resultText);
}

TEST (LSPFormattingTest, formattingFile_ShouldFormatTheFile_withBadToken) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/formatting", "params": {"textDocument": {"uri": "file://./examples/scanner/bad_tokens.material"},"options": {"tabSize": 2, "insertSpaces": true, "insertFinalNewline": true}}})");
    // exit request
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})");

    // read file
    std::string initialCode = test_utils::readFile("./examples/scanner/bad_tokens.material");
    std::string expectedCode = test_utils::readFile("./examples/lsp/formatting_bad_tokens.material");

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").is_array());

    std::vector<edit > edits = extractRanges(j.at("result"));
    std::string resultCode = test_utils::applyModifications(initialCode, edits);

//    std::cout << "----------------" << std::endl;
//    std::cout << resultCode << std::endl;
//    std::cout << "----------------" << std::endl;

    ASSERT_EQ(expectedCode, resultCode);
}
