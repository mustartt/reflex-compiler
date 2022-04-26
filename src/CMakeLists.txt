cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Source)
add_subdirectory(Lexer)
add_subdirectory(AST)
add_subdirectory(Parser)
add_subdirectory(ASTPrinter)

#add_subdirectory(Types)
#add_subdirectory(TypeChecker)
#add_subdirectory(CodeGen)

add_executable(reflex main.cpp)

#target_link_libraries(reflex ${llvm_libs} ast parser ast_printer types typechecker codegen)
target_link_libraries(reflex ${llvm_libs} lexer ast)
