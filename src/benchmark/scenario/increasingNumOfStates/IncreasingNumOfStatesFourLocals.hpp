#pragma once

#include <cstdint>

#include "IncreasingNumOfStatesBase.hpp"

namespace benchmark::scenario
{
class IncreasingNumOfStatesFourLocals : public IncreasingNumOfStatesBase
{
    static constexpr uint16_t numOfLocals{4};

public:
    IncreasingNumOfStatesFourLocals()
    {
        parameters.minNumOfLocals = numOfLocals;
        parameters.maxNumOfLocals = numOfLocals;
    }
};
} // namespace benchmark::scenario
