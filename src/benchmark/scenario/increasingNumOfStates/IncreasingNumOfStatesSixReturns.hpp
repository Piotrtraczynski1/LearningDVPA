#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesSixReturns : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfReturns{6};

public:
    IncreasingNumOfStatesSixReturns()
    {
        parameters.minNumOfReturns = numOfReturns;
        parameters.maxNumOfReturns = numOfReturns;
    }
};
} // namespace benchmark::scenario
