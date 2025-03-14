include(../../../../cmake/macros.cmake)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_test_target(TestCfg TestCfg.cpp)
    add_test_target(TestCalculator TestCalculator.cpp)
endif()
