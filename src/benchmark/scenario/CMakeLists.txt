add_library(scenario STATIC
    ../../Tester.cpp
    ./increasingNumOfStates/IncreasingNumOfStatesBase.cpp
    ./increasingNumOfStates/IncreasingNumOfStatesAndCalls.cpp
    ./increasingNumOfStates/IncreasingNumOfStatesAndReturns.cpp
    ./increasingNumOfStates/IncreasingNumOfStatesAndLocals.cpp
)

target_link_libraries(scenario PRIVATE utils common generator learner teacher)
