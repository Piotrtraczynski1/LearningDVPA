#pragma once

#include <array>
#include <cstdint>

#include "benchmark/scenario/srs/BaseSrsScenario.hpp"

namespace benchmark::scenario
{
class CommutationIncreasingNumOfStates : public BaseSrsScenario
{
    static constexpr uint16_t numOfIterationsIn1Dim{5};
    static const std::array<std::pair<uint16_t, uint16_t>, numOfIterationsIn1Dim> dim1Steps;
    uint16_t dim1Step{0};

public:
    uint16_t getNumOfIterationsIn1Dim() override;
    uint16_t getDim1Details() override;
    void runSingleIteration() override;
    void prepareNextIterationDim1() override;
    void resetDim1() override;
};
} // namespace benchmark::scenario