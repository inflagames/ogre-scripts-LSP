cmake_minimum_required(VERSION 3.27)
project(ogre_scripts_LSP)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)


set(CMAKE_CXX_STANDARD 23)

add_library(ogre_scripts_LSP STATIC
        core/src/scanner.cpp
        core/src/parser.cpp
)

target_link_libraries(ogre_scripts_LSP PRIVATE nlohmann_json::nlohmann_json)

# testing
enable_testing()
add_subdirectory(tests)
