add_subdirectory(common)
add_subdirectory(teacher)
add_subdirectory(learner)
add_subdirectory(mainComponent)

if(BUILD_TESTS)
    enable_testing()
endif()