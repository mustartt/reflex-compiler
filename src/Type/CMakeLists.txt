cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        type
        Type.cpp
        Type.h
        TypeContext.cpp
        TypeContext.h
)

set_target_properties(type PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(type PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(type ${llvm_libs} ast)
