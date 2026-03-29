#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesSixCalls : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfCalls{6};

public:
    IncreasingNumOfStatesSixCalls()
    {
        parameters.minNumOfCalls = numOfCalls;
        parameters.maxNumOfCalls = numOfCalls;
    }
};
} // namespace benchmark::scenario
