#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesEightReturns : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfReturns{8};

public:
    IncreasingNumOfStatesEightReturns()
    {
        parameters.minNumOfReturns = numOfReturns;
        parameters.maxNumOfReturns = numOfReturns;
    }
};
} // namespace benchmark::scenario
