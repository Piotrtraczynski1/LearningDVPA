#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesTenCalls : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfCalls{10};

public:
    IncreasingNumOfStatesTenCalls()
    {
        parameters.minNumOfCalls = numOfCalls;
        parameters.maxNumOfCalls = numOfCalls;
    }
};
} // namespace benchmark::scenario
