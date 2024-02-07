//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"
#include "nlohmann/json.hpp"

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

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

    std::vector<edit > edits;
    for (auto it: j.at("result")) {
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

    nlohmann::json j = nlohmann::json::parse(test_utils::extractJson(outMock.str(), 2));
    std::cout << nlohmann::to_string(j) << std::endl;

    ASSERT_TRUE(j.contains("id"));
    ASSERT_TRUE(j.contains("result"));
    ASSERT_TRUE(j.at("result").is_array());

    std::vector<edit > edits;
    for (auto it: j.at("result")) {
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

    std::vector<edit > edits;
    for (auto it: j.at("result")) {
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

    std::vector<edit > edits;
    for (auto it: j.at("result")) {
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

    std::string resultText = test_utils::applyModifications(initialCode, edits);

//    std::cout << "----------------" << std::endl;
//    std::cout << resultText << std::endl;
//    std::cout << "----------------" << std::endl;

    ASSERT_EQ(resultCode, resultText);
}
