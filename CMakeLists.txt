cmake_minimum_required(VERSION 3.14)

project(D--)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LLVM_LINK_COMPONENTS
  Analysis
  Core
  ExecutionEngine
  InstCombine
  Object
  OrcJIT
  Passes
  RuntimeDyld
  ScalarOpts
  Support
  native
)

# Set main project directories
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tests")

include_directories("${INCLUDE_DIR}")
add_subdirectory("${SRC_DIR}")
add_subdirectory("${TEST_DIR}")

# Set compiler flags for release/debug builds
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
