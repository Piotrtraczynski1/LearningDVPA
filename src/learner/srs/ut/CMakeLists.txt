include(../../../../cmake/macros.cmake)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_test_target(TestSpecialSymbolAdder TestSpecialSymbolAdder.cpp utils common teacher srs)
    add_test_target(TestSrsChecker TestSrsChecker.cpp utils common teacher srs)
endif()
