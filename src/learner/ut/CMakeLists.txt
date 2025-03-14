include(../../../cmake/macros.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_test_target(TestAutomataGenerator TestAutomataGenerator.cpp)
    add_test_target(TestLearner TestLearner.cpp)
endif()
