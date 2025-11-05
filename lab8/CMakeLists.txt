cmake_minimum_required(VERSION 3.10)

find_package(LLVM 14 REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS} include)
link_directories(${LLVM_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(constraint
  src/Constraint.cpp
  src/Extractor.cpp
  src/Utils.cpp
  )

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(constraint ${llvm_libs})
