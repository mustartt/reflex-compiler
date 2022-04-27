cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

set(gtest_SOURCE_DIR lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(unit_tests
        test.cpp
        Lexer/TokenTest.cpp
        Type/TypeContextTest.cpp Type/TypeTest.cpp)

target_link_libraries(unit_tests PRIVATE gtest gtest_main)
target_link_libraries(unit_tests PRIVATE lexer type)
