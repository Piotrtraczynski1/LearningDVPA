#pragma once

#include <cstdint>
#include <memory>

#include "common/Stack.hpp"
#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "teacher/AutomataCombiner.hpp"
#include "teacher/EmptinessChecker.hpp"
#include "utils/log.hpp"

namespace teacher
{
class Teacher
{
public:
    Teacher(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> automaton, const uint16_t numCallsArg,
        const uint16_t numReturnsArg, const uint16_t numLocalsArg,
        const uint16_t stackSymbolsNumber);

    bool membershipQuery(const common::Word &word) const;
    common::Stack stackContentQuery(const common::Word &word) const;
    std::shared_ptr<common::Word> equivalenceQuery(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis) const;

private:
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa;
    std::shared_ptr<teacher::AutomataCombiner<AutomatonKind::Combined>> automataCombiner;
    std::shared_ptr<teacher::EmptinessChecker> emptinessChecker;
};
} // namespace teacher