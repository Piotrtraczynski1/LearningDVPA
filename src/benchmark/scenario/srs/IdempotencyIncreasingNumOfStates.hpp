#pragma once

#include "benchmark/scenario/srs/BaseSrsScenario.hpp"

namespace benchmark::scenario
{
class IdempotencyIncreasingNumOfStates : public BaseSrsScenario
{
    static constexpr uint16_t numOfIterationsIn1Dim{25}; // TODO

public:
    uint16_t getNumOfIterationsIn1Dim() override;
    uint16_t getDim1Details() override;
    void runSingleIteration() override;
};
} // namespace benchmark::scenario