add_library(srsGenerator STATIC
    CombinedGenerator.cpp
    CommentsGenerator.cpp
)

target_link_libraries(srsGenerator PRIVATE common utils learner teacher)