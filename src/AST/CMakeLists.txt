cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(ast
        Ast.h
        AstNodes.h
        AstNodes.cpp
        AstVisitor.h
        AstContextManager.h
        Operator.cpp
        Operator.h)

set_target_properties(ast PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
