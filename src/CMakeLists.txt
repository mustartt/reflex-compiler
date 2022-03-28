cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Ast)
add_subdirectory(AstPrinter)
add_subdirectory(Lexer)
add_subdirectory(Parser)
add_subdirectory(Types)
add_subdirectory(TypeChecker)
add_subdirectory(CodeGen)

add_executable(reflex main.cpp)

target_link_libraries(reflex ${llvm_libs} ast parser ast_printer types typechecker codegen)
