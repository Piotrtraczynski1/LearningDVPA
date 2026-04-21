#pragma once

#include <cstdint>
#include <string>

#include "TesterParameters.hpp"
#include "benchmark/scenario/srs/BaseSrsScenario.hpp"
#include "utils/log.hpp"

namespace benchmark::scenario
{
class AccuracyScenarioBase : public BaseSrsScenario
{
    static constexpr int seed{42};
    const std::string dim1Name{"numOfFailedTests"};

protected:
    static constexpr uint16_t numOfTestsInSingleIteration{10000};

public:
    AccuracyScenarioBase()
    {
#ifndef RANDOM_EQUIVALENCE_QUERY
        ERR("This scenario requires random equivalence Query. Recompile with -randomEQ");
        exit(1);
#endif

        parameters.minNumOfStates = 5;
        parameters.maxNumOfStates = 12;
        parameters.minNumOfCalls = 2;
        parameters.maxNumOfCalls = 5;
        parameters.minNumOfLocals = 2;
        parameters.maxNumOfLocals = 5;
        parameters.minNumOfReturns = 2;
        parameters.maxNumOfReturns = 5;
        parameters.minNumOfStackSymbols = 2;
        parameters.maxNumOfStackSymbols = 5;
        parameters.numOfRandomTestingWords = 0;
        parameters.maxTestingWordLength = 200;
        parameters.useEquivalenceCheckToValidateOutput = true;
    }

    int getSeed() override;
    void prepareNextIterationDim1() override;
    uint16_t getNumOfTestsInSingleIteration() override;
    uint16_t getNumOfIterationsIn1Dim() override;
    void resetDim1() override;
    std::string getDim1Name() override;
};
} // namespace benchmark::scenario