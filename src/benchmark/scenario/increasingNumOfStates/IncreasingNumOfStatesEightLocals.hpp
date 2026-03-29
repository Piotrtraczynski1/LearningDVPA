#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesEightLocals : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfLocals{8};

public:
    IncreasingNumOfStatesEightLocals()
    {
        parameters.minNumOfLocals = numOfLocals;
        parameters.maxNumOfLocals = numOfLocals;
    }
};
} // namespace benchmark::scenario
