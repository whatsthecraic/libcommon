cmake_minimum_required(VERSION 3.10)

# Be verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

# Prefer Clang over GCC
set(CMAKE_C_COMPILER_NAMES clang gcc icc cc)
set(CMAKE_CXX_COMPILER_NAMES clang++ g++ icpc c++ cxx)

project(common)
set(CMAKE_CXX_STANDARD 17)

# Search for project modules in /build-aux/
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/build-aux/")

# Helpers
include(GoogleTest)
include(LibBacktrace)
include(SQLite3)

# Sources, it defines the library common
add_subdirectory(src obj)

add_library(libcommon INTERFACE)
target_link_libraries(libcommon INTERFACE common)
target_include_directories(libcommon INTERFACE "${CMAKE_SOURCE_DIR}/src")

# Tests
add_subdirectory(test)

## Create the final executable
#add_executable(cxx_database main.cpp)
#target_link_libraries(cxx_database common)

