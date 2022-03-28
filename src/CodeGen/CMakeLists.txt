cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM Include Dir: ${LLVM_INCLUDE_DIRS}")

# After resolving symlink /usr/include/llvm-12/ on wsl2
set(RESOLVED_LLVM_INCLUDE_DIR "/usr/include/llvm-12/")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${RESOLVED_LLVM_INCLUDE_DIR})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_library(codegen
        ClassInterfaceCodeGen.h
        ClassInterfaceCodeGen.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader)
target_link_libraries(codegen ${llvm_libs} ast types)

set_target_properties(codegen PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(codegen PUBLIC ${LLVM_INCLUDE_DIRS})
target_include_directories(codegen PUBLIC ${RESOLVED_LLVM_INCLUDE_DIR})



