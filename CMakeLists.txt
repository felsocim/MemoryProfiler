cmake_minimum_required(VERSION 3.4.3)
project(MemoryProfilerPass)

ADD_DEFINITIONS(-std=c++11)

SET(CMAKE_CXX_FLAGS "-fno-rtti")
  
# Find LLVM and add its definitions, libraries directory
# and includes directory
set(LLVM_INSTALL ${CMAKE_SOURCE_DIR}/../llvm-5.0.0-install-release)
message(STATUS "Looking for LLVM in ${LLVM_INSTALL}")
find_package(LLVM
  REQUIRED
  CONFIG
  PATHS ${LLVM_INSTALL}
  NO_DEFAULT_PATH
)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM includes: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM libraries: ${LLVM_LIBRARY_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Create the pass as a library
add_library(LLVMMemoryProfiler MODULE MemoryProfiler.cpp)
