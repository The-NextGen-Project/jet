cmake_minimum_required(VERSION 3.0)
project(jet)


# Import
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/import)


# Force C++ 11 Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(tests)


add_executable(nextgen main.cpp

        # Import Support Libraries. These are the only headers with value definitions.
        import/nextgen/support/core.h
        import/nextgen/support/panic.h


        # Link the executables with all resolved symbols in the C++ files.
        lib/support/str.cpp
        lib/support/io.cpp
        lib/lex/lexer.cpp)