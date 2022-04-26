cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        ast_printer
        AstPrinter.h
        AstPrinter.cpp
)

target_link_libraries(ast_printer LINK_PUBLIC ast)

set_target_properties(ast_printer PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(ast_printer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
