#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesTenReturns : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfReturns{10};

public:
    IncreasingNumOfStatesTenReturns()
    {
        parameters.minNumOfReturns = numOfReturns;
        parameters.maxNumOfReturns = numOfReturns;
    }
};
} // namespace benchmark::scenario
