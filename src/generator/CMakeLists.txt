add_library(generator STATIC
    RandomGenerator.cpp
    XMLGenerator.cpp
)

target_link_libraries(generator PRIVATE common utils common learner teacher)
 