cmake_minimum_required(VERSION 3.0)
project(tests)



# Setup Google Test Compilation Cases

add_executable(tests

        # Link Support
        ../src/allocator.cpp
        ../src/util/str.cpp

        # Lexical Analysis
        #../src/Lex/Lexer.cpp
        ../src/diagnostics.cpp
        ../src/lexer.cpp


        # Parsing
        ../src/parser.cpp

        ../src/x64-codegen.cpp
#
#        ################################
#
#        ## Tests
        test-main.cpp
#
        lex-test.cpp
        parse-test.cpp
        x64-test.cpp


        # FMT
        ../third_party/fmt/src/format.cc
        ../third_party/fmt/src/os.cc
)


include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

set_target_properties(tests PROPERTIES COMPILE_FLAGS "${cxx_strict}")
