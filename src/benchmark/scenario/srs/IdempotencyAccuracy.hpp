#pragma once

#include "benchmark/scenario/srs/AccuracyScenarioBase.hpp"

namespace benchmark::scenario
{
class IdempotencyAccuracy : public AccuracyScenarioBase
{
    uint16_t dim1Details{0};

public:
    IdempotencyAccuracy()
    {
        parameters.minNumOfStates = 10;
        parameters.maxNumOfStates = 24;
    }

    uint16_t getDim1Details() override;
    std::string getGeneratorName() const override;
    SingleTestResult runSingle(uint32_t seed) override;
};
} // namespace benchmark::scenario
