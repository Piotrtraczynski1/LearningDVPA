include(../../../cmake/macros.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_test_target(TestVPA TestVPA.cpp)
    add_test_target(TestStack TestStack.cpp)
endif()
