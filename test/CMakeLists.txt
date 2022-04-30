cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

set(gtest_SOURCE_DIR lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(unit_tests
        test.cpp
        Lexer/TokenTest.cpp
        Type/TypeContextTest.cpp
        Type/TypeTest.cpp
        LexicalScope/LexicalScopeTest.cpp
        LexicalScope/LexicalContextTest.cpp SemanticPassTest/ForwardPassTest.cpp Type/InterfaceTypeTest.cpp Type/ClassTypeTest.cpp)

file(GLOB TEST_FILES "TestFiles/*.test")
message(STATUS "Copying test files ${TEST_FILES}")
file(COPY ${TEST_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/TestFiles)

target_link_libraries(unit_tests PRIVATE gtest_main lexer type parser lexcontext)
