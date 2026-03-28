#include <cstdint>
#include <queue>

#include "learner/SrsChecker.hpp"
#include "utils/log.hpp"

namespace learner
{
SrsChecker::SrsChecker(
    std::shared_ptr<Selectors> selectors_, std::shared_ptr<TestWords> testWords_, Srs srs_,
    teacher::Teacher &oracle_)
    : selectors{selectors_}, testWords{testWords_}, srs{srs_}, oracle{oracle_}
{
}

common::Word
SrsChecker::check(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis) const
{
    for (const auto &srsRule : srs)
    {
        for (uint16_t it = 0; it < selectors->size(); it++)
        {
            common::Word lhs{(*selectors)[it] + srsRule.l};
            common::Word rhs{(*selectors)[it] + srsRule.r};

            common::Word res{checkConfigurationsConsistency(hypothesis, lhs, rhs)};

            if (res != common::Word{})
            {
                IMP("SRS WORKS");
                return res;
            }
        }
    }

    return common::Word{};
}

common::Word SrsChecker::checkConfigurationsConsistency(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const common::Word &lhs,
    const common::Word &rhs) const
{
    hypothesis->checkWord(lhs);
    common::transition::State s1{hypothesis->state};
    common::Stack stack1{hypothesis->stack};
    hypothesis->checkWord(rhs);
    common::transition::State s2{hypothesis->state};
    common::Stack stack2{hypothesis->stack};
    if (s1 == s2 and stack1 == stack2)
    {
        return common::Word{};
    }

    std::queue<common::Word> vCandidates{};
    vCandidates.push(common::Word{});

    while (not vCandidates.empty())
    {
        common::Word vCandidate{vCandidates.front()};
        vCandidates.pop();

        if (hypothesis->checkWord(lhs + vCandidate) != hypothesis->checkWord(rhs + vCandidate))
        {
            if (hypothesis->checkWord(lhs + vCandidate) != oracle.membershipQuery(lhs + vCandidate))
            {
                std::cout << "CA found (1): lhs: " << lhs << ", rhs: " << rhs
                          << ", vCandidate: " << vCandidate << std::endl;
                return lhs + vCandidate;
            }
            std::cout << "CA found (2): lhs: " << lhs << ", rhs: " << rhs
                      << ", vCandidate: " << vCandidate << std::endl;
            return rhs + vCandidate;
        }

        if (vCandidate.size() > 20)
        {
            ERR("vCandidate.size() > 20");
            continue;
        }

        for (uint16_t it = 0; it < numOfCalls; it++)
        {
            vCandidates.push(vCandidate + common::Word{common::symbol::CallSymbol{it}});
        }
        for (uint16_t it = 0; it < numOfLocals; it++)
        {
            vCandidates.push(vCandidate + common::Word{common::symbol::LocalSymbol{it}});
        }
        for (uint16_t it = 0; it < numOfReturns; it++)
        {
            vCandidates.push(vCandidate + common::Word{common::symbol::ReturnSymbol{it}});
        }
    }

    return common::Word{};
}
} // namespace learner