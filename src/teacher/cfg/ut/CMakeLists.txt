include(../../../../cmake/macros.cmake)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_test_target(TestCfg TestCfg.cpp utils common cfg)
    add_test_target(TestCalculator TestCalculator.cpp utils common cfg)
endif()
