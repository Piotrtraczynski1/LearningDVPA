include(../../../cmake/macros.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_test_target(TestParser TestParser.cpp)
endif()
