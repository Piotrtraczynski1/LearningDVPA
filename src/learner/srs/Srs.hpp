#pragma once

#include <vector>

#include "common/Word.hpp"

namespace learner::srs
{
struct SrsRuleWithParams
{
    struct Side
    {
        common::Word left{};
        common::Word right{};
        bool takesParams{true};
    };

    Side left{};
    Side right{};
};

struct SrsRule
{
    common::Word left{};
    common::Word right{};
};

using Srs = std::vector<SrsRuleWithParams>;
} // namespace learner::srs