cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

set(gtest_SOURCE_DIR lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(unit_tests
        test.cpp
        Lexer/TokenTest.cpp
        Type/TypeContextTest.cpp
        Type/TypeTest.cpp LexicalScope/LexicalScopeTest.cpp LexicalScope/LexicalContextTest.cpp)

target_link_libraries(unit_tests PRIVATE gtest_main lexer type lexcontext)
