cmake_minimum_required(VERSION 3.10)
project(fuzzing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
  src/DictionaryMutation.cpp
  src/PowerSchedule.cpp
  src/Utils.cpp
  )

add_executable(dictionary_application_check EXCLUDE_FROM_ALL
  test/dictionary_application_public.cpp
  src/DictionaryMutation.cpp
  )

add_executable(power_schedule_check EXCLUDE_FROM_ALL
  test/power_schedule_public.cpp
  src/PowerSchedule.cpp
  )

add_llvm_library(FuzzingAnalysis MODULE
  src/Register.cpp
  src/InstrumentDivision.cpp
  src/InstrumentComparisons.cpp
  src/BuildDictionary.cpp
  )

add_library(runtime MODULE
  lib/runtime.c
  lib/cmplog.c
  )
