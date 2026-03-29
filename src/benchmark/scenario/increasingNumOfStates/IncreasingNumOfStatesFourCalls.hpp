#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesFourCalls : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfCalls{4};

public:
    IncreasingNumOfStatesFourCalls()
    {
        parameters.minNumOfCalls = numOfCalls;
        parameters.maxNumOfCalls = numOfCalls;
    }
};
} // namespace benchmark::scenario
