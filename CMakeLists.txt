cmake_minimum_required(VERSION 3.11)

project(MyProject VERSION 1.0)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_HARD_DIAGNOSTICS "Enable debug checks" OFF)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)

if (BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )

    FetchContent_MakeAvailable(googletest)

    enable_testing()
endif()

if (BUILD_BENCHMARKS)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
      googlebenchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG v1.9.1
    )

    FetchContent_MakeAvailable(googlebenchmark)
endif()


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
add_subdirectory(src)

if(NOT BUILD_BENCHMARKS)
    add_executable(run src/main.cpp src/Tester.cpp)
    target_link_libraries(run PRIVATE common teacher learner utils generator) #parser
else()
    add_executable(run_bench src/benchmark/benchMain.cpp src/Tester.cpp)
    target_link_libraries(run_bench PRIVATE common teacher learner utils generator benchmark::benchmark)
endif()