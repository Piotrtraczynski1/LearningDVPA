cmake_minimum_required(VERSION 3.11)

project(MyProject VERSION 1.0)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_HARD_DIAGNOSTICS "Enable debug checks" OFF)
option(RANDOM_EQUIVALENCE_QUERY "Enable random equivalence query" OFF)
option(MIXED_EQUIVALENCE_QUERY "Use random SRS checks and exact learner equivalence queries" OFF)

if(RANDOM_EQUIVALENCE_QUERY AND MIXED_EQUIVALENCE_QUERY)
    message(FATAL_ERROR "RANDOM_EQUIVALENCE_QUERY and MIXED_EQUIVALENCE_QUERY cannot both be enabled")
endif()

if (BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )

    FetchContent_MakeAvailable(googletest)

    enable_testing()
endif()

if(RANDOM_EQUIVALENCE_QUERY)
    add_compile_definitions(RANDOM_EQUIVALENCE_QUERY)
endif()

if(MIXED_EQUIVALENCE_QUERY)
    add_compile_definitions(MIXED_EQUIVALENCE_QUERY)
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

add_executable(run src/main.cpp src/Tester.cpp)
target_link_libraries(run PRIVATE common teacher learner utils generator helper benchmark) #parser
