#pragma once

#include "benchmark/scenario/srs/BaseSrsScenario.hpp"

namespace benchmark::scenario
{
class IdempotencyIncreasingNumOfStates : public BaseSrsScenario
{
    static constexpr uint16_t numOfIterationsIn1Dim{14};

public:
    uint16_t getNumOfIterationsIn1Dim() override;
    uint16_t getDim1Details() override;
    std::string getGeneratorName() const override;
    SingleTestResult runSingle(uint32_t seed) override;

    void prepareNextIterationDim1() override;
};
} // namespace benchmark::scenario
