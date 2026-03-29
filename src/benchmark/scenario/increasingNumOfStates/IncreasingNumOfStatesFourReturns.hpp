#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesFourReturns : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfReturns{4};

public:
    IncreasingNumOfStatesFourReturns()
    {
        parameters.minNumOfReturns = numOfReturns;
        parameters.maxNumOfReturns = numOfReturns;
    }
};
} // namespace benchmark::scenario
