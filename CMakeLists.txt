cmake_minimum_required(VERSION 3.26)
project(ogre_scripts_LSP_lib)

# define application version here
add_definitions(-DAPP_VERSION="v0.0.3")

include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/src/*.h")
add_library(ogre_scripts_LSP_lib STATIC ${SOURCES_FILES} ${HEADER_FILES})

add_executable(ogre_scripts_LSP main.cpp)


target_link_libraries(ogre_scripts_LSP_lib PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(ogre_scripts_LSP ogre_scripts_LSP_lib)

# link standard libraries into the app as static
target_link_libraries(ogre_scripts_LSP -static-libgcc -static-libstdc++ -static -lpthread)

# testing
enable_testing()
add_subdirectory(tests)
