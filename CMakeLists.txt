cmake_minimum_required(VERSION 3.0)
project(jet)

# Import
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/import)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party)


# Force C++ 14 Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(tests)


add_executable(jet main.cpp


    # Import Support Libraries.
        lib/panic.cpp
        lib/allocator.cpp
        lib/str.cpp
        lib/io.cpp

        # Lexical Analysis
        lib/jet-lexer-v2.cpp
        lib/jet-diagnostics.cpp

        # Parsing
        lib/jet-parser.cpp

        # Codegen
        lib/jet-x64-assembler.cpp
        lib/jet-cgen.cpp

        # Semantic Analysis
        lib/jet-analyzer.cpp
        )

# Link against LLVM libraries
target_link_libraries(jet ${llvm_libs})
