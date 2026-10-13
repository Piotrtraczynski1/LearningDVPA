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
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa, const bool updateCounters);

#if defined(RANDOM_EQUIVALENCE_QUERY) || defined(MIXED_EQUIVALENCE_QUERY)
std::shared_ptr<common::Word> randomEquivalenceCheck(
    std::shared_ptr<teacher::AutomataCombiner<AutomatonKind::Combined>> automataCombiner,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa, uint32_t numOfRandomWords,
    uint16_t maxLengthRandomWord, bool updateCounters);
#endif
} // namespace teacher
