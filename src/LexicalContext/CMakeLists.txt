cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        lexcontext
        LexicalScope.cpp
        LexicalScope.h
        LexicalContext.cpp
        LexicalContext.h
        LexicalContextPass.cpp LexicalContextPass.h)

set_target_properties(lexcontext PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(lexcontext PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lexcontext ast type)
