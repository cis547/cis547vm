cmake_minimum_required(VERSION 3.10)

find_package(LLVM 8 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

option(USE_REFERENCE "Build with reference solution" OFF)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

if (USE_REFERENCE)
  add_llvm_library(DataflowPass MODULE
    src/DataflowAnalysis.cpp
    src/PointerAnalysis.cpp
    src/DivZeroAnalysis.cpp
    )

  target_link_libraries(DataflowPass RefDomain)

  add_library(RefDomain MODULE
    reference/RefDomain.cpp
    )
else (USE_REFERENCE)
  add_llvm_library(DataflowPass MODULE
    src/DataflowAnalysis.cpp
    src/DivZeroAnalysis.cpp
    src/Domain.cpp
    src/PointerAnalysis.cpp
    )
endif (USE_REFERENCE)
