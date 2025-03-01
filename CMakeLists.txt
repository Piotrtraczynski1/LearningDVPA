cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_COMPILER "g++")

add_subdirectory(src)

add_executable(run src/main.cpp)

target_link_libraries(run PRIVATE format)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    enable_testing()
endif()