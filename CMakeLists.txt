cmake_minimum_required(VERSION 3.20)
project(jet)

# Import
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party)


set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED OFF)
#set(CMAKE_CXX_EXTENSIONS OFF)


include_directories(third_party/fmt/include)
add_subdirectory(tests)
add_subdirectory(third_party/fmt)


link_libraries(fmt)
add_executable(jet main.cpp


        # Import Support Libraries.
        src/allocator.cpp
        src/util/str.cpp

        # Lexical Analysis
        src/lexer.cpp
        src/diagnostics.cpp

        # Parsing
        src/parser.cpp

        # Codegen
        src/x64-codegen.cpp
        src/c-codegen.cpp

        # Semantic Analysis
        src/analyzer.cpp
        )
