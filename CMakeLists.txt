cmake_minimum_required(VERSION 3.6)

include(${CMAKE_SOURCE_DIR}/cmake/project_settings.cmake)

project(simd-puzzles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE)

# set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# project options
option(ENABLE_SIMD_AVX2 "Enable AVX2 vector extensions." ON)

if(ENABLE_SIMD_AVX2)
    add_definitions(-DENABLE_SIMD_AVX2)
endif()

# unit testing
add_subdirectory(${PROJECT_SOURCE_DIR}/extern/googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(src)
add_subdirectory(test)