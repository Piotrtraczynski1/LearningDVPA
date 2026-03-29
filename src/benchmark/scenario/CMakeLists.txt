add_library(scenario STATIC
    ../../Tester.cpp
    ./increasingNumOfStates/IncreasingNumOfStatesBase.cpp
)

target_link_libraries(scenario PRIVATE utils common generator learner teacher)
