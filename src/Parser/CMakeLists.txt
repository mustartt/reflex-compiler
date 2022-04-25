cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        parser
        Parser.h
        Parser.cpp
        ErrorHandler.cpp
        ErrorHandler.h
        ParsingError.cpp
        ParsingError.h
        ParserType.cpp
)

target_link_libraries(parser LINK_PUBLIC ast lexer)

set_target_properties(parser PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
