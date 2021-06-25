cmake_minimum_required(VERSION 3.0)
project(tests)



# Setup Google Test Compilation Cases
add_executable(tests

        # Link Support
        ../lib/Diagnostics.cpp
        ../lib/Support/Panic.cpp
        ../lib/Support/Allocator.cpp
        ../lib/Support/String.cpp
        ../lib/Support/IO.cpp

        # Lexical Analysis
        ../lib/Lex/Lexer.cpp

        # Parsing
        ../lib/Parse/Parser.cpp

        ################################

        # Support
        Support/ListTest.cpp

        # Lex
        Lex/LexTest.cpp

        # Parse
        Parse/ParseTest.cpp

)

set(gtest_force_shared_crt ON CACHE BOOL "MSVC defaulting to CRT" FORCE)

add_subdirectory(third_party/googletest)

target_compile_definitions(gtest
        PUBLIC
        GTEST_LANG_CXX11
        GTEST_HASH_TR1_TUPLE=0
        )

include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

set_target_properties(tests PROPERTIES COMPILE_FLAGS "${cxx_strict}")
target_link_libraries(tests gtest gtest_main)