#pragma once

#include <cstdint>
#include <vector>

#include "common/Word.hpp"

namespace learner::srs
{
struct WellMatchedNode
{
    std::vector<uint16_t> nextState;
    common::Word word;
    uint16_t score;
};
}