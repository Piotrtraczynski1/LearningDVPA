#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesTenLocals : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfLocals{10};

public:
    IncreasingNumOfStatesTenLocals()
    {
        parameters.minNumOfLocals = numOfLocals;
        parameters.maxNumOfLocals = numOfLocals;
    }
};
} // namespace benchmark::scenario
