cmake_minimum_required(VERSION 3.21)

add_executable(wirezero_tests
    test_basic.cpp
)

target_link_libraries(wirezero_tests
    PRIVATE
        wirezero
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(wirezero_tests)