if(BUILD_TESTS)
    add_subdirectory(ut)
endif()


add_library(teacher STATIC
    AutomataCombiner.cpp
    EmptinessChecker.cpp
    Teacher.cpp
    EquivalenceCheck.cpp
)

target_link_libraries(teacher PRIVATE common utils)
