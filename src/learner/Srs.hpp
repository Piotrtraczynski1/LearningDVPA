#pragma once

#include <vector>

#include "common/Word.hpp"

namespace learner
{
struct SrsRule
{
    common::Word l;
    common::Word r;
};

using Srs = std::vector<SrsRule>;
} // namespace learner