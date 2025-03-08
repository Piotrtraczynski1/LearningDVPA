cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_COMPILER "g++")

include_directories(${CMAKE_SOURCE_DIR}/src)

if(BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(src)

add_executable(run src/main.cpp)

target_link_libraries(run PRIVATE common teacher learner parser)

option(BUILD_TESTS "Build tests" OFF)