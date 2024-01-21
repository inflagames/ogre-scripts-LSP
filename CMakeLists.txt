cmake_minimum_required(VERSION 3.27)
project(js_ai)

set(CMAKE_CXX_STANDARD 23)

add_library(js_ai STATIC core/src/library.cpp
        core/inc/token.h)

# testing
enable_testing()
add_subdirectory(tests)
