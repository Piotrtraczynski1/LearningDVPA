add_library(generator STATIC
    Generator.cpp
    RandomGenerator.cpp
    CdaGenerator.cpp
    SeVpaGenerator.cpp
    MeVpaGenerator.cpp
    ECdaGenerator.cpp
)

target_link_libraries(generator PRIVATE common utils common learner teacher)
 