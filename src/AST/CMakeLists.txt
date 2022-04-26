cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        ast
        AST.h
        AST.cpp
        ASTDeclaration.cpp
        ASTDeclaration.h
        ASTStatement.cpp
        ASTStatement.h
        ASTExpression.cpp
        ASTExpression.h
        ASTType.cpp
        ASTType.h
        ASTUtils.cpp
        ASTUtils.h
        ASTLiteral.cpp
        ASTLiteral.h
        ASTContext.cpp
        ASTContext.h
        Operator.cpp
        Operator.h
        Utils/Generics.h ASTVisitor.h)

set_target_properties(ast PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ast PUBLIC source)
