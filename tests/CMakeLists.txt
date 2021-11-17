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
#
#        ../src/x64-codegen.cpp
#
#        ################################
#
#        ## Tests
        test-main.cpp
#
        lex-test.cpp
        parse-test.cpp
#        x64-test.cpp
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