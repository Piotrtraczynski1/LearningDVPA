#pragma once

#include "utils/Constants.hpp"
#include <cstdint>

namespace common::transition
{
enum State : uint16_t
{
    INVALID = utils::MaxNumOfCombinedAutomatonStates - 1
};
} // namespace common::transition