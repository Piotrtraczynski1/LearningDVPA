if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_library(srs STATIC
    SrsChecker.cpp
)

target_link_libraries(srs PRIVATE common)