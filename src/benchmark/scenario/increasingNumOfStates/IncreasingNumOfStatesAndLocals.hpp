#pragma once

#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesAndLocals : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfIterationsIn2Dim{7};
    const std::string dim2Name{"numOfLocals"};

public:
    uint16_t getNumOfIterationsIn2Dim() override;
    std::string getDim2Name() override;
    uint16_t getDim2Details() override;
    void prepareNextIterationDim2() override;
    void resetDim2() override;
};
} // namespace benchmark::scenario