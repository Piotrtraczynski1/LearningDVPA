#pragma once

#include "benchmark/scenario/srs/AccuracyScenarioBase.hpp"

namespace benchmark::scenario
{
class CommutationAccuracy : public AccuracyScenarioBase
{
    uint16_t dim1Details{0};

public:
    CommutationAccuracy()
    {
        parameters.minNumOfStackSymbols = 2;
        parameters.maxNumOfStackSymbols = 2;
        parameters.minNumOfStates = 2;
        parameters.maxNumOfStates = 5;
        parameters.secondDvpaNumOfStates = 3;
    }

    uint16_t getDim1Details() override;
    void runSingleIteration() override;
};
} // namespace benchmark::scenario