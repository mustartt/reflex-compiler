cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        codegen
        ClassDataLayout.cpp
        ClassDataLayout.h
)

target_link_libraries(codegen LINK_PUBLIC ast type)

set_target_properties(codegen PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
