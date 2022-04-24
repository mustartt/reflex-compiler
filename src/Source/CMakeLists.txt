cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        source
        TokenType.h
        TokenType.cpp
        Token.h
        Token.cpp
        SourceManager.h
        SourceManager.cpp
)

set_target_properties(source PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
