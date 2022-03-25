cmake_minimum_required(VERSION 3.16)

project(reflex)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)
add_subdirectory(src)
enable_testing()
add_subdirectory(test)

