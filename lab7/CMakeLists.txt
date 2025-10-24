cmake_minimum_required(VERSION 3.10)
project(pointer_analysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 14 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})


add_llvm_library(DivZeroPass MODULE
  src/PointerAnalysis.cpp
  src/DivZeroAnalysis.cpp
  src/Transfer.cpp
  src/ChaoticIteration.cpp
  src/Domain.cpp
  src/Utils.cpp
)
