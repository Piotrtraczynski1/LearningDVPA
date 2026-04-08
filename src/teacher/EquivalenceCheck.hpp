#pragma once

#include <cstdint>
#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "teacher/AutomataCombiner.hpp"
#include "teacher/EmptinessChecker.hpp"

namespace teacher
{
std::shared_ptr<common::Word> equivalenceCheck(
    std::shared_ptr<teacher::AutomataCombiner<AutomatonKind::Combined>> automataCombiner,
    std::shared_ptr<teacher::EmptinessChecker> emptinessChecker,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa);
} // namespace teacher
