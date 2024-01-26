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

add_executable(test_run
        Test.cpp
        SimpleScannerTest.cpp
        LSPTest.cpp
        LSPInitializeTest.cpp
)

target_link_libraries(test_run gtest gtest_main ogre_scripts_LSP_lib)

include(GoogleTest)
gtest_discover_tests(test_run)
