add_subdirectory(callDriven)
add_subdirectory(srs)

add_library(generator STATIC
    Generator.cpp
    RandomGenerator.cpp
)

target_link_libraries(generator PRIVATE common utils learner teacher callDriven srsGenerator)
 