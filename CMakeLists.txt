cmake_minimum_required(VERSION 3.16)
project(reflex)

set(CMAKE_CXX_STANDARD 20)

add_executable(reflex src/main.cpp src/Lexer/TokenType.h src/Lexer/Token.cpp src/Lexer/Token.h src/Lexer/SourceManager.cpp src/Lexer/SourceManager.h src/Lexer/Lexer.cpp src/Lexer/Lexer.h src/Lexer/TokenDesc.h src/Lexer/TokenDesc.cpp)
