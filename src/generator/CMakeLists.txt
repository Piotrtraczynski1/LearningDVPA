add_library(generator STATIC
    RandomGenerator.cpp
    XMLGenerator.cpp
    eCDAGenerator.cpp
)

target_link_libraries(generator PRIVATE common utils common learner teacher)
 