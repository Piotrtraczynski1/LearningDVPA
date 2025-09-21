cmake_minimum_required(VERSION 3.10)

project(MyProject VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_COMPILER "g++")

option(ENABLE_HARD_DIAGNOSTICS "Enable debug checks" OFF)

if (ENABLE_HARD_DIAGNOSTICS)
    add_compile_options(
        -O2 -g
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    add_link_options(
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
    add_compile_definitions(
        _GLIBCXX_DEBUG
        _GLIBCXX_ASSERTIONS
    )
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

if(BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(src)

add_executable(run src/main.cpp src/Tester.cpp)

target_link_libraries(run PRIVATE common teacher learner utils generator) #parser

option(BUILD_TESTS "Build tests" OFF)