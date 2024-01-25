cmake_minimum_required(VERSION 3.27)
project(ogre_scripts_LSP)

set(CMAKE_CXX_STANDARD 23)

add_library(ogre_scripts_LSP STATIC
        core/src/scanner.cpp
        core/src/parser.cpp
)

# testing
enable_testing()
add_subdirectory(tests)
