# Setup Google Test Compilation Cases
add_executable(tests)

# Add All Tests
target_sources(tests
        PRIVATE
        lex/LexTest.cpp
)
set_target_properties(tests PROPERTIES COMPILE_FLAGS "${cxx_strict}")
target_link_libraries(tests gtest gtest_main)