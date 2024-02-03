project(tests_run)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(COPY examples DESTINATION ${CMAKE_BINARY_DIR}/tests)

add_library(test_utils STATIC
        utils/utils.h
        utils/utils.cpp
)
set_target_properties(test_utils PROPERTIES LINKER_LANGUAGE CXX)

add_executable(test_run
        Test.cpp
        SimpleScannerTest.cpp
        SimpleParserTest.cpp
        LSPTest.cpp
        LSPInitializeTest.cpp
        ErrorHandlingParserTest.cpp
        LSPFormattingTest.cpp
        LSPGoToDefinitionTest.cpp
)

target_link_libraries(test_run gtest gtest_main nlohmann_json::nlohmann_json ogre_scripts_LSP_lib test_utils)

include(GoogleTest)
gtest_discover_tests(test_run)
