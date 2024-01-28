//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/lsp_server.h"

TEST (LSPTest, readHeader_ShouldReadCorrectlyTheHeader) {
    auto *lsp = new lsp_server();
    std::istringstream osMock(
            "Content-Length: 18\r\nS1: naa\r\nS2:hah  ahah\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n");

    Action msg = lsp->readHeaders(osMock);

    ASSERT_EQ(18, msg.contentLength);
    ASSERT_EQ("application/vscode; charset=utf-8", msg.contentType);
}

TEST (LSPTest, readBody_ShouldReadCorrectlyTheBody_Shutdown) {
    auto *lsp = new lsp_server();
    std::istringstream osMock("{\"id\": 1234,\"method\": \"shutdown\"}");

    Action msg = lsp->readContent({39, "", new RequestMessage()}, osMock);

    ASSERT_TRUE(msg.message != nullptr);
    ASSERT_EQ("1234", ((RequestMessage *) msg.message)->id);
    ASSERT_EQ("shutdown", ((RequestMessage *) msg.message)->method);
}

TEST (LSPTest, readBody_ShouldReadCorrectlyTheBody_Exit) {
    auto *lsp = new lsp_server();
    std::istringstream osMock("{\"id\": 1234,\"method\": \"exit\"}");

    Action msg = lsp->readContent({35, "", new RequestMessage()}, osMock);

    ASSERT_TRUE(msg.message != nullptr);
    ASSERT_EQ("1234", ((RequestMessage *) msg.message)->id);
    ASSERT_EQ("exit", ((RequestMessage *) msg.message)->method);
}

TEST (LSPTest, readHeadBody_ShouldReadCorrectlyTheHeadAndBody) {
    auto *lsp = new lsp_server();
    std::istringstream osMock(
            "Content-Length: 59\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\":\"2.0\",\"id\": 1234,\"method\": \"shutdown\"}");

    Action msg = lsp->readHeaders(osMock);

    ASSERT_EQ(59, msg.contentLength);
    ASSERT_EQ("application/vscode; charset=utf-8", msg.contentType);

    msg = lsp->readContent(msg, osMock);

    ASSERT_TRUE(msg.message != nullptr);
    ASSERT_EQ("2.0", ((RequestMessage *) msg.message)->jsonrpc);
    ASSERT_EQ("1234", ((RequestMessage *) msg.message)->id);
    ASSERT_EQ("shutdown", ((RequestMessage *) msg.message)->method);
}
