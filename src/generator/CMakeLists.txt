add_subdirectory(callDriven)

add_library(generator STATIC
    Generator.cpp
    RandomGenerator.cpp
)

target_link_libraries(generator PRIVATE common utils learner teacher callDriven)
 