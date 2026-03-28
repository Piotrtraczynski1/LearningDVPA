if(BUILD_TESTS)
    add_subdirectory(ut)
endif()

add_subdirectory(srs)

add_library(learner STATIC
    Learner.cpp
    AutomataGenerator.cpp
)

target_link_libraries(learner PRIVATE common)
target_link_libraries(learner PRIVATE srs)
target_link_libraries(learner PRIVATE teacher) 