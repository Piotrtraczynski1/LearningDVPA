add_library(srsGenerator STATIC
    CommutativeGenerator.cpp
    CancellationGenerator.cpp
    IdempotencyGenerator.cpp
    XmlGenerator.cpp
)

target_link_libraries(srsGenerator PRIVATE common utils learner teacher callDriven)