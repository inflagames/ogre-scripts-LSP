cmake_minimum_required(VERSION 3.26)
project(ogre_scripts_LSP_lib)

include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_STANDARD 23)

add_library(ogre_scripts_LSP_lib STATIC
        core/src/scanner.cpp
        core/inc/scanner.h
        core/src/parser.cpp
        core/inc/parser.h
        core/src/lsp_server.cpp
        core/inc/lsp_server.h
        core/inc/lsp_protocol.h
        core/inc/ast_tree.h
        core/inc/exceptions.h
        core/inc/logs.h
        core/src/logs.cpp
        core/src/formatter.cpp
        core/inc/formatter.h
        core/src/goto.cpp
        core/inc/goto.h
        core/src/program_arg.cpp
        core/inc/program_arg.h
)

add_executable(ogre_scripts_LSP main.cpp)


target_link_libraries(ogre_scripts_LSP_lib PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(ogre_scripts_LSP ogre_scripts_LSP_lib)

# link standard libraries into the app as static
target_link_libraries(ogre_scripts_LSP -static-libgcc -static-libstdc++ -static -lpthread)

# testing
enable_testing()
add_subdirectory(tests)
