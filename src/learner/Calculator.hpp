#pragma once

#include <memory>

#include "common/Word.hpp"
#include "learner/Selectors.hpp"
#include "learner/TestWords.hpp"
#include "teacher/Teacher.hpp"

namespace learner
{
class Calculator
{
    std::shared_ptr<Selectors> selectors;
    std::shared_ptr<TestWords> testWords;
    const teacher::Teacher &oracle;

public:
    Calculator(
        const teacher::Teacher &teacher, std::shared_ptr<Selectors> s,
        std::shared_ptr<TestWords> tw);

    bool isRightCongruence(const common::Word &a, const common::Word &b) const;
    bool areDistinguishable(const common::Word &a, const common::Word &b) const;
    uint16_t findOrAddSuccessor(const common::Word &candidate);
    uint16_t addSelector(const common::Word &selector);

private:
    uint16_t findEquivalentSelector(const common::Word &word);
};
} // namespace learner
