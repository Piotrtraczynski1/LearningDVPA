add_library(learner STATIC
    Learner.cpp
)

target_link_libraries(learner PRIVATE common)
target_link_libraries(learner PRIVATE teacher)