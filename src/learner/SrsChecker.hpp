#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "learner/Selectors.hpp"
#include "learner/Srs.hpp"
#include "learner/TestWords.hpp"
#include "teacher/Teacher.hpp"

namespace learner
{
class SrsChecker
{
    std::shared_ptr<Selectors> selectors;
    std::shared_ptr<TestWords> testWords;
    Srs srs;
    teacher::Teacher &oracle;

    const uint16_t numOfCalls{6};
    const uint16_t numOfReturns{6};
    const uint16_t numOfLocals{5};

public:
    SrsChecker(
        std::shared_ptr<Selectors> selectors_, std::shared_ptr<TestWords> testWords_, Srs srs_,
        teacher::Teacher &oracle_);

    common::Word check(const std::shared_ptr<common::VPA> hypothesis) const;

private:
    common::Word checkConfigurationsConsistency(
        const std::shared_ptr<common::VPA> hypothesis, const common::Word &lhs,
        const common::Word &rhs) const;
};
} // namespace learner
