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
        parameters.minSecondDvpaNumOfStates = 2;
        parameters.maxSecondDvpaNumOfStates = 4;
    }

    uint16_t getDim1Details() override;
    std::string getGeneratorName() const override;
    SingleTestResult runSingle(uint32_t seed) override;
};
} // namespace benchmark::scenario
