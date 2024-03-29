cmake_minimum_required(VERSION 3.0)
project(tests)



# Setup Google Test Compilation Cases
add_executable(tests

        # Link Support
        ../lib/panic.cpp
        ../lib/allocator.cpp
        ../lib/str.cpp
        ../lib/io.cpp

        # Lexical Analysis
        #../lib/Lex/Lexer.cpp
        ../lib/jet-diagnostics.cpp
        ../lib/jet-lexer-v2.cpp


        # Parsing
        ../lib/jet-parser.cpp

        ../lib/jet-x64-assembler.cpp

        ################################

        ## Tests
        test-main.cpp

        lex-test.cpp
        parse-test.cpp
        x64-test.cpp
)

#set(gtest_force_shared_crt ON CACHE BOOL "MSVC defaulting to CRT" FORCE)

#add_subdirectory(third_party/googletest)

#target_compile_definitions(gtest
#        PUBLIC
#        GTEST_LANG_CXX11
#        GTEST_HASH_TR1_TUPLE=0
#        )

include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

set_target_properties(tests PROPERTIES COMPILE_FLAGS "${cxx_strict}")
#target_link_libraries(tests gtest gtest_main)