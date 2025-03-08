include(../../../../cmake/macros.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_test_target(TestState TestState.cpp)
    add_test_target(TestArgument TestArgument.cpp)
    add_test_target(TestCoArgument TestCoArgument.cpp)
    add_test_target(TestTransition TestTransition.cpp)
endif()
