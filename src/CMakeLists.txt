add_subdirectory(format)
add_subdirectory(common)
add_subdirectory(mainComponent)

if(BUILD_TESTS)
    enable_testing()
endif()