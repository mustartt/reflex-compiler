cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        lexer
        Lexer.h
        Lexer.cpp
        TokenDesc.cpp
        TokenDesc.h
)

set_target_properties(lexer PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(lexer PUBLIC source)