include(../../../../cmake/macros.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_test_target(TestCoArgument TestCoArgument.cpp)
    add_test_target(TestTransition TestTransition.cpp)
endif()
