cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

add_library(
        astprinter
        AstPrinter.h
        AstPrinter.cpp
)

target_link_libraries(astprinter LINK_PUBLIC ast type)

set_target_properties(astprinter PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(astprinter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
