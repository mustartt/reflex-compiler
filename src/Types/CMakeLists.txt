cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(types
        Type.h
        Type.cpp
        TypeContextManager.h
        TypeContextManager.cpp
        TypeParser.cpp
        TypeParser.h)

target_link_libraries(types LINK_PUBLIC ast codegen)

set_target_properties(types PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
