include(../../../../cmake/macros.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_test_target(TestAutomataConverter TestAutomataConverter.cpp)
    add_test_target(TestSrsChecker TestSrsChecker.cpp)
endif()
