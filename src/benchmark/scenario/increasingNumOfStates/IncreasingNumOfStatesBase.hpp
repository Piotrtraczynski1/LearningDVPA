/*
The base scenario consists of 19 iterations, where the number of states increases from 2 to 20.
Each iteration contains 100 randomly generated automata.
*/

#pragma once

#include "TesterParameters.hpp"
#include "benchmark/scenario/Scenario.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesBase : public Scenario
{
    static constexpr int seed{50};
    static constexpr uint16_t numOfIterationsIn1Dim{8};
    static constexpr uint16_t numOfTestsInSingleIteration{200};
    const std::string dim1Name{"numOfStates"};

protected:
    TesterParameters parameters{
        .minNumOfStates = 2,
        .maxNumOfStates = 2,
        .minNumOfCalls = 3,
        .maxNumOfCalls = 3,
        .minNumOfLocals = 3,
        .maxNumOfLocals = 3,
        .minNumOfReturns = 3,
        .maxNumOfReturns = 3,
        .minNumOfStackSymbols = 5,
        .maxNumOfStackSymbols = 5,
        .numOfRandomTestingWords = 0,
        .maxTestingWordLength = 30,
        .density = 0.8, // TODO density = 1 drastically decreased number of queries
        .acceptingStatesDensity = 0.3,
        .savePassedTestData = false,
        .supervisedMode = false,
        .supervisedTestMaxDuration = std::chrono::seconds{600}};

public:
    int getSeed() override;
    uint16_t getNumOfIterationsIn1Dim() override;
    std::string getDim1Name() override;
    uint16_t getDim1Details() override;
    uint16_t getNumOfTestsInSingleIteration() override;
    void runSingleIteration() override;
    void prepareNextIterationDim1() override;
    void resetDim1() override;
};
} // namespace benchmark::scenario