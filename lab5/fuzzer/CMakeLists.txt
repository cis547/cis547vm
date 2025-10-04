cmake_minimum_required(VERSION 3.10)

include_directories(${LLVM_INCLUDE_DIRS} include)

add_executable(fuzzer
  src/Fuzzer.cpp
  src/Utils.cpp
  )