#pragma once

#include <memory>

#include "common/VPA.hpp"

namespace learner::srs
{
struct ConvertedAutomata
{
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> lAutomaton;
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> rAutomaton;
};
} // namespace learner::srs