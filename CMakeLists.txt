cmake_minimum_required(VERSION 3.0)
project(jet)


# Import
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/import)


# Force C++ 11 Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(tests)


add_executable(jet main.cpp


    # Import Support Libraries.
    lib/support/panic.cpp
    lib/support/allocator.cpp
    lib/support/str.cpp
    lib/support/io.cpp

    # Lexical Analysis
    lib/lex/lexer.cpp
)