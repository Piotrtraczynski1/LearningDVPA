add_library(srsScenarios STATIC
    ../../../Tester.cpp
    BaseSrsScenario.cpp
    CancellationIncreasingNumOfStates.cpp
    CommutationIncreasingNumOfStates.cpp
    IdempotencyIncreasingNumOfStates.cpp
    AccuracyScenarioBase.cpp
    CancellationAccuracy.cpp
    CommutationAccuracy.cpp
    IdempotencyAccuracy.cpp
)

target_link_libraries(srsScenarios PRIVATE utils common generator learner teacher)
