include(../../../cmake/macros.cmake)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_test_target(TestConverter TestConverter.cpp utils common teacher)
    add_test_target(TestTeacher TestTeacher.cpp utils common teacher)
endif()
