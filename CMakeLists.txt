cmake_minimum_required(VERSION 3.13)
project(SarcasmLangCompiler)

# ✅ Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# For M1 Macs, help CMake find LLVM
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/llvm" ${CMAKE_PREFIX_PATH})

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set LLVM definitions and include directories
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Find LLVM libraries
llvm_map_components_to_libnames(llvm_libs 
    support core irreader executionengine interpreter 
    mc mcjit bitwriter target native nativecodegen)

# Create the executable
add_executable(sarcasmlang compiler.cpp)

# Link against LLVM libraries
target_link_libraries(sarcasmlang ${llvm_libs})

# M1-specific settings
if(APPLE AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
    message(STATUS "Building for Apple Silicon (M1/M2)")
endif()
