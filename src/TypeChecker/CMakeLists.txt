cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(typechecker
        Utils/TopSort.h
        AstInterfaceDeclAnnotator.cpp
        AstInterfaceDeclAnnotator.h SemanticError.cpp SemanticError.h AstClassDeclAnnotator.cpp AstClassDeclAnnotator.h Utils/TypeCheckerUtils.cpp Utils/TypeCheckerUtils.h AstMemberAnnotator.cpp AstMemberAnnotator.h)

target_link_libraries(typechecker PUBLIC ast)
target_link_libraries(typechecker PUBLIC types)

set_target_properties(ast PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
