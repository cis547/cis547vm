cmake_minimum_required(VERSION 3.10)
project(Fuzzer)

find_package(LLVM 14 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})


add_executable(fuzzer
  src/Fuzzer.cpp
  src/Utils.cpp
  )

add_llvm_library(InstrumentPass MODULE
  src/Instrument.cpp
  )

add_library(runtime MODULE
  lib/runtime.c
  )
