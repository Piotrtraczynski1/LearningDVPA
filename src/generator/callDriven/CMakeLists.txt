add_library(callDriven STATIC
    CdaGenerator.cpp
    SeVpaGenerator.cpp
    MeVpaGenerator.cpp
    ECdaGenerator.cpp
)

target_link_libraries(callDriven PRIVATE common utils learner teacher)