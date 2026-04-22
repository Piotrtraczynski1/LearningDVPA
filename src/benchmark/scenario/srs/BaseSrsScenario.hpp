#pragma once

#include "TesterParameters.hpp"
#include "benchmark/scenario/Scenario.hpp"

namespace benchmark::scenario
{
class BaseSrsScenario : public Scenario
{
    static constexpr int seed{50};
    static constexpr uint16_t numOfIterationsIn2Dim{2};
    const std::string dim1Name{"numOfStates"};
    const std::string dim2Name{"useSrs"};

protected:
    static constexpr uint16_t numOfTestsInSingleIteration{10};
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
        .density = 0.8,
        .acceptingStatesDensity = 0.3,
        .numOfModules = 5,
        .useSrs = false,
        .secondDvpaNumOfStates = 2,
        .savePassedTestData = false,
        .supervisedMode = false,
        .supervisedTestMaxDuration = std::chrono::seconds{600}};

public:
    int getSeed() override;
    uint16_t getNumOfIterationsIn2Dim() override;
    uint16_t getNumOfTestsInSingleIteration() override;
    std::string getDim2Name() override;
    uint16_t getDim2Details() override;
    void prepareNextIterationDim2();
    void resetDim2();

    virtual void prepareNextIterationDim1();
    virtual void resetDim1();
    virtual std::string getDim1Name();
};
} // namespace benchmark::scenario