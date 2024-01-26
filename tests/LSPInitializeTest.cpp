//
// Created by Guillermo on 25.05.23.
//

#include "gtest/gtest.h"

#include "../core/inc/LspServer.h"

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

    InitializeParams *p = ((InitializeParams *)rm->params);
    ASSERT_EQ(31, p->processId);
    ASSERT_EQ("/some/that", p->rootUri);
}
