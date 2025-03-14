add_library(generator STATIC
    Generator.cpp
    Tester.cpp
)

target_link_libraries(generator PRIVATE common utils common learner teacher)
