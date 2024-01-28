cmake_minimum_required(VERSION 3.27)
project(ogre_scripts_LSP_lib)

include(FetchContent)
FetchContent_Declare(
        json
        #        URL https://github.com/nlohmann/json/releases/download/v3.11.3/include.zip
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_STANDARD 23)

add_library(ogre_scripts_LSP_lib STATIC
        core/src/scanner.cpp
        core/inc/scanner.h
        core/src/parser.cpp
        core/inc/parser.h
        core/src/LspServer.cpp
        core/inc/LspServer.h
        core/inc/LspProtocol.h
        core/inc/ast_tree.h
)

add_executable(ogre_scripts_LSP main.cpp)

target_link_libraries(ogre_scripts_LSP_lib PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(ogre_scripts_LSP ogre_scripts_LSP_lib)

# testing
enable_testing()
add_subdirectory(tests)
