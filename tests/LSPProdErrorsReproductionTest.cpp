#include "gtest/gtest.h"
#include "nlohmann/json.hpp"

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"

using namespace OgreScriptLSP;

TEST (LSPProdErrorsReproductionTest, shouldExecuteWithoutErrors) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData;

    // read example
    std::ifstream file("./examples/lsp/error_example_from_prod.txt");
    std::string line;
    while (!file.eof()) {
        std::getline(file, line);
        inputData += test_utils::getMessageStr(line);
    }

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

    std::cout << outMock.str() << std::endl;

    ASSERT_EQ(5, test_utils::countJson(outMock.str()));
}

TEST (LSPProdErrorsReproductionTest, shouldExecuteWithoutErrors_newMaterialAtTheEnd) {
    GTEST_SKIP();
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData;

    // read example
    std::ifstream file("./examples/lsp/error_example_from_prod2.txt");
    std::string line;
    while (!file.eof()) {
        std::getline(file, line);
        inputData += test_utils::getMessageStr(line);
    }

    // system in/out mocks
    std::istringstream inMock(inputData);
    std::ostringstream outMock;

    lsp->runServer(outMock, inMock);

//    std::cout << outMock.str() << '\n';
    ASSERT_EQ(5, test_utils::countJson(outMock.str()));
}
