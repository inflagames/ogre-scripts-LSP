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
)

add_executable(ogre_scripts_LSP main.cpp)

if (UNIX)
    # Compile relese directly on bin folder
#    set_target_properties(ogre_scripts_LSP PROPERTIES
#            RUNTIME_OUTPUT_DIRECTORY_RELEASE /usr/local/bin
#    )
endif (UNIX)
if (WIN32)
    # todo: install
endif (WIN32)

target_link_libraries(ogre_scripts_LSP_lib PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(ogre_scripts_LSP ogre_scripts_LSP_lib)

# testing
enable_testing()
add_subdirectory(tests)
