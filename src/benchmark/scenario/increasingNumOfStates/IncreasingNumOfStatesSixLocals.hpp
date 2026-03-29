#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesSixLocals : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfLocals{6};

public:
    IncreasingNumOfStatesSixLocals()
    {
        parameters.minNumOfLocals = numOfLocals;
        parameters.maxNumOfLocals = numOfLocals;
    }
};
} // namespace benchmark::scenario
