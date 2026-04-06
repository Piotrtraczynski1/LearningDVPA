add_library(srsGenerator STATIC
    CombinedGenerator.cpp
)

target_link_libraries(srsGenerator PRIVATE common utils learner teacher)