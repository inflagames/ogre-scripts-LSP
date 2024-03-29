#include "gtest/gtest.h"

#include "../src/lsp_server.h"

using namespace OgreScriptLSP;

TEST (LSPTest, readHeader_ShouldReadCorrectlyTheHeader) {
    auto *lsp = new LspServer();
    std::istringstream osMock(
            "Content-Length: 18\r\nS1: naa\r\nS2:hah  ahah\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n");

    Action msg = lsp->readHeaders(osMock);

    ASSERT_EQ(18, msg.contentLength);
    ASSERT_EQ("application/vscode; charset=utf-8", msg.contentType);
}

TEST (LSPTest, readBody_ShouldReadCorrectlyTheBody_Shutdown) {
    auto *lsp = new LspServer();
    std::istringstream osMock("{\"id\": 1234,\"method\": \"shutdown\"}");

    Action msg = {39, "", new RequestMessage()};
    lsp->readContent(msg, osMock);

    ASSERT_TRUE(msg.message != nullptr);
    ASSERT_EQ("1234", ((RequestMessage *) msg.message.get())->id);
    ASSERT_EQ("shutdown", ((RequestMessage *) msg.message.get())->method);
}

TEST (LSPTest, readBody_ShouldReadCorrectlyTheBody_Exit) {
    auto *lsp = new LspServer();
    std::istringstream osMock("{\"id\": 1234,\"method\": \"exit\"}");

    Action msg = {35, "", new RequestMessage()};
    lsp->readContent(msg, osMock);

    ASSERT_TRUE(msg.message != nullptr);
    ASSERT_EQ("1234", ((RequestMessage *) msg.message.get())->id);
    ASSERT_EQ("exit", ((RequestMessage *) msg.message.get())->method);
}

TEST (LSPTest, readHeadBody_ShouldReadCorrectlyTheHeadAndBody) {
    auto *lsp = new LspServer();
    std::istringstream osMock(
            "Content-Length: 59\r\nContent-Type: application/vscode; charset=utf-8\r\n\r\n{\"jsonrpc\":\"2.0\",\"id\": 1234,\"method\": \"shutdown\"}");

    Action msg = lsp->readHeaders(osMock);

    ASSERT_EQ(59, msg.contentLength);
    ASSERT_EQ("application/vscode; charset=utf-8", msg.contentType);

    lsp->readContent(msg, osMock);

    ASSERT_TRUE(msg.message != nullptr);
    ASSERT_EQ("2.0", ((RequestMessage *) msg.message.get())->jsonrpc);
    ASSERT_EQ("1234", ((RequestMessage *) msg.message.get())->id);
    ASSERT_EQ("shutdown", ((RequestMessage *) msg.message.get())->method);
}
