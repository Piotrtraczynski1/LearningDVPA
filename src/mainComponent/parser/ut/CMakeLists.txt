include(../../../../cmake/macros.cmake)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_test_target(TestParser TestParser.cpp)
endif()
