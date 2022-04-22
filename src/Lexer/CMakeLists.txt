cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(lexer
        Lexer.h
        Lexer.cpp
        Source.h
        Source.cpp
        Token.h
        Token.cpp
        TokenDesc.cpp
        TokenDesc.h
        TokenType.cpp
        TokenType.h
        SourceManager.cpp SourceManager.h)

set_target_properties(lexer PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
