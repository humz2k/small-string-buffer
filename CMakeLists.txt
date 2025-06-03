cmake_minimum_required(VERSION 3.24)

# for printing variables
include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

project(SMALLSTRING LANGUAGES CXX)

add_library(smallstring INTERFACE)

target_include_directories(smallstring INTERFACE include)

if (${PROJECT_IS_TOP_LEVEL})
add_executable(example example.cpp)
target_link_libraries(example PUBLIC smallstring)
add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark PUBLIC smallstring)


FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
enable_testing()
file( GLOB TEST_SOURCES tests/*.cpp )
foreach( sourcefile ${TEST_SOURCES} )
    get_filename_component( name ${sourcefile} NAME_WE )
    add_executable( ${name} ${sourcefile} )
    target_link_libraries( ${name} smallstring GTest::gtest_main )
    gtest_discover_tests(${name})
endforeach( sourcefile ${TEST_SOURCES} )

endif()