cmake_minimum_required(VERSION 3.24)

# for printing variables
include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(SMALLSTRING LANGUAGES CXX)

add_library(smallstring INTERFACE)

target_include_directories(smallstring INTERFACE include)

if (${PROJECT_IS_TOP_LEVEL})
add_executable(test test.cpp)
target_link_libraries(test PUBLIC smallstring)
endif()