cmake_minimum_required(VERSION 3.16)
project(reflex)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(AST)
add_subdirectory(AstPrinter)
add_subdirectory(Lexer)
add_subdirectory(Parser)

add_executable(reflex main.cpp)

target_link_libraries(reflex LINK_PUBLIC ast)
target_link_libraries(reflex LINK_PUBLIC parser)
target_link_libraries(reflex LINK_PUBLIC ast_printer)


