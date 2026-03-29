#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesEightCalls : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfCalls{8};

public:
    IncreasingNumOfStatesEightCalls()
    {
        parameters.minNumOfCalls = numOfCalls;
        parameters.maxNumOfCalls = numOfCalls;
    }
};
} // namespace benchmark::scenario
