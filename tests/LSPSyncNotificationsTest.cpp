#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <synchapi.h>
#endif

#include "gtest/gtest.h"

#include "../core/inc/lsp_server.h"
#include "utils/utils.h"


using namespace OgreScriptLSP;

/**
 * @brief Initiates the execution of the server functionality associated with the provided LspServer instance.
 * 
 * This function triggers the execution of the server functionality implemented within the specified LspServer instance.
 * It delegates the execution to the `runServer` method of the LspServer instance, passing the provided output and input
 * streams for communication.
 * 
 * @param lsp A pointer to the LspServer instance whose server functionality is to be executed.
 * @param oos An output stream used for communication with the server.
 * @param ios An input stream used for communication with the server.
 * 
 * @return void
 * 
 * @remarks
 *  - This function serves as a simple wrapper around the `runServer` method of the LspServer class, abstracting the
 *    details of server initialization and execution.
 *  - The provided output and input streams are utilized by the server implementation to communicate with external
 *    entities, such as clients or other server components.
 *  - Users should ensure that the provided LspServer instance is properly initialized and configured before invoking
 *    this function to avoid undefined behavior.
 * 
 * @dependencies
 *  - This function relies on the existence and correct implementation of the LspServer class, including the `runServer`
 *    method for server execution.
 *  - It also requires valid output and input stream objects (`std::ostream` and `std::istream`) for communication
 *    with the server.
 * 
 * @note
 *  - Developers should verify that the LspServer instance is appropriately configured to handle communication via the
 *    provided output and input streams.
 *  - This function is designed to abstract away the details of server initialization and execution, providing a
 *    convenient interface for starting the server functionality.
 *  - It is recommended to handle exceptions or errors that may occur during server execution within the `runServer`
 *    method or through appropriate error handling mechanisms.
 */
void runServer(LspServer *lsp, std::ostream &oos, std::istream &ios) {
    lsp->runServer(oos, ios);
}

/**
 * @brief Waits for a specified size to be reached in the parsers container of the provided LspServer instance.
 * 
 * This function waits until the size of the parsers container of the given LspServer instance matches the specified
 * size value. It continuously checks the size of the parsers container until either the desired size is reached or
 * a timeout occurs.
 * 
 * @param lsp A pointer to the LspServer instance whose parsers container size is being monitored.
 * @param sizeValue An integer representing the desired size of the parsers container.
 * 
 * @return void
 * 
 * @remarks
 *  - The function relies on the test_utils::getTimeNow() function to obtain the current time for timeout comparison.
 *  - It utilizes the TIMEOUT_MS constant to determine the maximum duration to wait for the parsers container size
 *    to reach the specified value.
 *  - If the size of the parsers container does not reach the desired value within the timeout period, the function
 *    prints an error message indicating a timeout and asserts that the condition has failed, terminating the program.
 * 
 * @dependencies
 *  - This function assumes the existence of a LspServer class with a member variable named "parsers" representing
 *    the container whose size is being monitored.
 *  - It also requires the definition and proper functionality of the test_utils::getTimeNow() function for time retrieval.
 *  - The TIMEOUT_MS constant must be appropriately defined to avoid premature timeouts or excessive waiting times.
 *  - The function utilizes the ASSERT_TRUE macro to terminate the program upon a timeout condition, assuming it is
 *    defined and configured correctly in the testing environment.
 * 
 * @note
 *  - Care should be taken to ensure that TIMEOUT_MS is set to a reasonable value to prevent overly long waiting times
 *    or premature timeouts.
 *  - Developers should verify that the LspServer class and its associated parsers container are correctly implemented
 *    and accessible through the provided pointer.
 *  - This function is primarily intended for use in testing scenarios to synchronize actions based on container size.
 *    It may not be suitable for general-purpose synchronization due to its reliance on a specific container size.
 */
void waitForSize(LspServer *lsp, int sizeValue) {
    uint64_t start = test_utils::getTimeNow();
    while (lsp->parsers.size() != sizeValue) {
        uint64_t current = test_utils::getTimeNow();
        if (current - start > TIMEOUT_MS) {
            std::cout << "Timeout waiting for size to be equals: " << sizeValue << ". Current value: "
                      << lsp->parsers.size() << std::endl;
            ASSERT_TRUE(false);
        }
    }
}

/**
 * @brief Suspends the execution of the current thread for a specified duration based on the operating system.
 * 
 * This function provides a cross-platform way to pause the execution of the current thread for a specified number
 * of milliseconds. The duration of suspension depends on the underlying operating system.
 * 
 * @param milliseconds The duration, in milliseconds, for which the thread should be suspended.
 * 
 * @return void
 * 
 * @remarks
 *  - On Windows, the suspension is achieved using the Sleep function from the Windows API.
 *  - On non-Windows platforms, such as Linux, the suspension is accomplished using the sleep function,
 *    where the argument represents the number of seconds to sleep. Therefore, the provided duration
 *    in milliseconds is converted to seconds by dividing it by 1000.
 *  - The behavior of this function is subject to the accuracy and granularity of the underlying operating system's
 *    timing facilities.
 *  - This function is not guaranteed to be precise, especially for short durations, and should not be relied upon
 *    for precise timing requirements.
 * 
 * @dependencies
 *  - This function requires conditional compilation based on the preprocessor directive _WIN32 to determine
 *    the appropriate sleep function to use.
 *  - On Windows, it depends on the Sleep function from the Windows API (synchapi.h).
 *  - On non-Windows platforms, it depends on the sleep function from the POSIX standard library (unistd.h).
 * 
 * @note
 *  - It is essential to ensure proper handling of the provided duration to avoid unexpected behavior,
 *    especially when dealing with platform-specific differences in timing mechanisms.
 *  - Developers should be cautious when using this function for precise timing requirements or in scenarios
 *    where strict timing accuracy is crucial.
 */
void sleepOS(size_t milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    sleep(milliseconds / 1000);
#endif
}

TEST (LSPSyncTest, clientSync_LspServerShouldOpenCloseChangeFiles_validateLspAfterEachSync) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    inputData += test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 500, "method": "textDocument/formatting", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"},"options": {"tabSize": 2, "insertSpaces": true}}})");

    // system in/out mocks
    auto *stringstreambuf = new test_utils::my_stringstreambuf(inputData);
    std::istream inMock(stringstreambuf);
    std::stringstream outMock;

    // run server on thread
    std::thread runServerThread(runServer, std::ref(lsp), std::ref(outMock), std::ref(inMock));

    // with the formatting, the parser map should be updated
    waitForSize(lsp, 1);

    // send close file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didClose", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 0);

    // open 2 files
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 1);
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}}})"));
    waitForSize(lsp, 1);
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didOpen", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_tab_size_4_spaces.material"}}})"));
    waitForSize(lsp, 2);

    // send close file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didClose", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_tab_size_4_spaces.material"}}})"));
    waitForSize(lsp, 1);

    // exit request
    stringstreambuf->appendStr(test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})"));
    stringstreambuf->setEof();

    // wait for thread
    runServerThread.join();
}

/**
 * This test is replicating a bug from the integration with JetBrains plugin
 */
TEST (LSPSyncTest, clientSync_ShouldReceiveDiagnostic) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // formatting request
    std::string fileToOpen = test_utils::readFile("./examples/lsp/formatting_programs_basic.material", true);
    inputData += test_utils::getMessageStr(
            "{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/didOpen\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/formatting_programs_basic.material\", \"text\": \"" +
            fileToOpen + "\"}}}"
    );

    std::cout << inputData << std::endl;

    // system in/out mocks
    auto *stringstreambuf = new test_utils::my_stringstreambuf(inputData);
    std::istream inMock(stringstreambuf);
    std::stringstream outMock;

    // run server on thread
    std::thread runServerThread(runServer, std::ref(lsp), std::ref(outMock), std::ref(inMock));

    // with the didOpen, the parser map should be updated
    waitForSize(lsp, 1);

    // send updated file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}, "contentChanges": [{"text": "fragment_program Game/Floor/ShaderFp glsl glsles {\n    source FloorFp.glsl\n}\n\nmaterial something {\n    technique {\n        pass {\n        }\n    }\n}\n\n\nmaterial kk : something {\n\n}\n\nvertex_program Game/Floor/ShaderVp glsl {\n    source FloorVp.glsl\n\n    default_params {\n        // assign samplers as required by GLSL\n        param_named normalMap int 0\n    }\n}\n"}]}})"
    ));
    while (stringstreambuf->currentChar() != WAITING_DATA);
    ASSERT_TRUE(lsp->running);

    // send updated file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}, "contentChanges": [{"text": "fragment_program Game/Floor/ShaderFp glsl glsles {\n    source FloorFp.glsl\n}\n\nmaterial something {\n    technique {\n        pass {\n        }\n    }\n}\n\n\nmaterial kk : something {j\n\n}\n\nvertex_program Game/Floor/ShaderVp glsl {\n    source FloorVp.glsl\n\n    default_params {\n        // assign samplers as required by GLSL\n        param_named normalMap int 0\n    }\n}\n"}]}})"
    ));
    while (stringstreambuf->currentChar() != WAITING_DATA);
    sleepOS(1);// sleep for a second
    ASSERT_TRUE(lsp->running);// at this point the server crash

    // exit request
    stringstreambuf->appendStr(test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})"));
    stringstreambuf->setEof();

    // wait for thread
    runServerThread.join();
}

/**
 * This test is replicating a bug from the integration with JetBrains plugin
 */
TEST (LSPSyncTest, clientSync_CoverServerCrashIssueInServer_validateBug) {
    auto *lsp = new LspServer();
    // initialize request
    std::string inputData = test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 1234, "method": "initialize", "params": {"processId": 31, "clientInfo": {"name": "client-name"}, "rootUri": "/some/that", "capabilities": {}}})");
    // initialized notification
    inputData += test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 456, "method": "initialized", "params": {}})");
    // open request
    std::string fileToOpen = test_utils::readFile("./examples/lsp/formatting_programs_basic.material", true);
    inputData += test_utils::getMessageStr(
            "{\"jsonrpc\": \"2.0\", \"id\": 500, \"method\": \"textDocument/didOpen\", \"params\": {\"textDocument\": {\"uri\": \"file://./examples/lsp/formatting_programs_basic.material\", \"text\": \"" +
            fileToOpen + "\"}}}"
    );

    std::cout << inputData << std::endl;

    // system in/out mocks
    auto *stringstreambuf = new test_utils::my_stringstreambuf(inputData);
    std::istream inMock(stringstreambuf);
    std::stringstream outMock;

    // run server on thread
    std::thread runServerThread(runServer, std::ref(lsp), std::ref(outMock), std::ref(inMock));

    // with the didOpen, the parser map should be updated
    waitForSize(lsp, 1);

    // send updated file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}, "contentChanges": [{"text": "fragment_program Game/Floor/ShaderFp glsl glsles {\n    source FloorFp.glsl\n}\n\nmaterial something {\n    technique {\n        pass {\n        }\n    }\n}\n\n\nmaterial kk : something {\n\n}\n\nvertex_program Game/Floor/ShaderVp glsl {\n    source FloorVp.glsl\n\n    default_params {\n        // assign samplers as required by GLSL\n        param_named normalMap int 0\n    }\n}\n"}]}})"
    ));
    while (stringstreambuf->currentChar() != WAITING_DATA);
    ASSERT_TRUE(lsp->running);

    // send updated file
    stringstreambuf->appendStr(test_utils::getMessageStr(
            R"({"jsonrpc": "2.0", "id": 98, "method": "textDocument/didChange", "params": {"textDocument": {"uri": "file://./examples/lsp/formatting_programs_basic.material"}, "contentChanges": [{"text": "fragment_program Game/Floor/ShaderFp glsl glsles {\n    source FloorFp.glsl\n}\n\nmaterial something {\n    technique {\n        pass {\n        }\n    }\n}\n\n\nmaterial kk : something {j\n\n}\n\nvertex_program Game/Floor/ShaderVp glsl {\n    source FloorVp.glsl\n\n    default_params {\n        // assign samplers as required by GLSL\n        param_named normalMap int 0\n    }\n}\n"}]}})"
    ));
    while (stringstreambuf->currentChar() != WAITING_DATA);
    sleepOS(1);// sleep for a second
    ASSERT_TRUE(lsp->running);// at this point the server crash

    // exit request
    stringstreambuf->appendStr(test_utils::getMessageStr(R"({"jsonrpc": "2.0", "id": 7345, "method": "exit"})"));
    stringstreambuf->setEof();

    // wait for thread
    runServerThread.join();
}
