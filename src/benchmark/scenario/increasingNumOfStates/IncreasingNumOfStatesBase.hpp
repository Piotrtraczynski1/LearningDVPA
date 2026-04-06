/*
The base scenario consists of 15 iterations, where the number of states increases from 2 to 16.
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
    static constexpr uint16_t numOfIterationsIn1Dim{18};
    static constexpr uint16_t numOfTestsInSingleIteration{100};
    const std::string dim1Name{"numOfStates"};

protected:
    TesterParameters parameters{
        .minNumOfStates = 2,
        .maxNumOfStates = 2,
        .minNumOfCalls = 2,
        .maxNumOfCalls = 2,
        .minNumOfLocals = 2,
        .maxNumOfLocals = 2,
        .minNumOfReturns = 2,
        .maxNumOfReturns = 2,
        .minNumOfStackSymbols = 2,
        .maxNumOfStackSymbols = 2,
        .numOfRandomTestingWords = 0,
        .maxTestingWordLength = 30,
        .density = 1, // TODO density = 1 drastically decreased number of queries
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