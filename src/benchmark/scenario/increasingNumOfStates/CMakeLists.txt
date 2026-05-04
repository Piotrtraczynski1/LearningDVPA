add_library(increasingNumOfStates STATIC
    ../../../Tester.cpp
    IncreasingNumOfStatesBase.cpp
    IncreasingNumOfStatesAndCalls.cpp
    IncreasingNumOfStatesAndReturns.cpp
    IncreasingNumOfStatesAndLocals.cpp
    IncreasingNumOfStatesAndStackSymbols.cpp
)

target_link_libraries(increasingNumOfStates PRIVATE utils common generator learner teacher)
