add_library(helper STATIC
    ExampleRun.cpp
    InputParser.cpp
)

target_link_libraries(helper PRIVATE common utils learner teacher generator)
 