cmake_minimum_required(VERSION 2.9)
project(nextgen)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/import)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(nextgen main.cpp

        # Import Support Libraries. These are the only headers with value definitions.
        import/nextgen/support/core.h
        import/nextgen/support/panic.h



        # Link the executables with all resolved symbols in the C++ files.
        lib/support/str.cpp
        lib/support/io.cpp
        lib/support/allocator.cpp
        lib/lex/lexer.cpp)