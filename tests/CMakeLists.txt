# Setup Google Test Compilation Cases
add_executable(tests)

set(gtest_force_shared_crt ON CACHE BOOL "MSVC defaulting to CRT" FORCE)

# Add All Tests
target_sources(tests
        PRIVATE
        lex/LexTest.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/third_party/googletest/googletest/include)

set_target_properties(tests PROPERTIES COMPILE_FLAGS "${cxx_strict}")
target_link_libraries(tests gtest gtest_main)