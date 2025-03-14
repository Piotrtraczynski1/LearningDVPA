#include <iostream> //tmp

#include "learner/Learner.hpp"
#include "utils/TimeMarker.hpp"

namespace learner
{
std::shared_ptr<common::VPA> Learner::run()
{
    TIME_MARKER("Learner::run");
    std::shared_ptr<common::Word> counterExample;

    IMP("[Learner]: Learner running");
    while (true)
    {
        hypothesis = generator.generate();

        counterExample = oracle.equivalenceQuery(hypothesis);

        if (counterExample->empty())
        {
            IMP("[Learner]: VPA found");
            return hypothesis;
        }
        handleCounterExample(counterExample);
    }
}

void Learner::handleCounterExample(std::shared_ptr<common::Word> counterExample)
{
    TIME_MARKER("handleCounterExample");
    std::cout << "[Learner]: CounterExample: " << *counterExample << std::endl;

    for (uint16_t i = 0; i < counterExample->size(); i++)
    {
        common::Word v{counterExample->begin(), counterExample->begin() + i};
        common::Word a{1, (*counterExample)[i]};
        common::Word w{counterExample->begin() + i + 1, counterExample->end()};

        hypothesis->checkWord(v + a);
        if (oracle.stackContentQuery(v + a) != hypothesis->stack)
        {
            handleStackContentDiverges(v, a, w);
            return;
        }

        common::Word suffix{common::Word{hypothesis->stack} + w};
        common::transition::State state{hypothesis->state};
        common::Word selector{(*selectors)[state]};

        if (oracle.membershipQuery(v + a + suffix) != oracle.membershipQuery(selector + suffix))
        {
            handleSuffixesMismatch(v, a, w);
            return;
        }
    }

    ERR("Counter example partition not found membershipQuery()=%d, hypothesis->checkWord()=%d",
        oracle.membershipQuery(*counterExample), hypothesis->checkWord(*counterExample));
    exit(1);
}

void Learner::handleSuffixesMismatch(
    const common::Word &v, const common::Word &a, const common::Word &w)
{
    testWords->addWord(common::Word{oracle.stackContentQuery(v + a)} + w);
    addNewSelectorIfNeeded(v + a);
}

void Learner::handleStackContentDiverges(
    const common::Word &v, const common::Word &a, const common::Word &w)
{
    for (uint16_t i = 0; i < v.size(); i++)
    {
        common::Word v1{v.begin(), v.begin() + i};
        common::Word b{1, v[i]};
        common::Word v2{v.begin() + i + 1, v.end()};

        hypothesis->checkWord(v1);
        common::Word q{(*selectors)[hypothesis->state]}; // not needed
        hypothesis->checkWord(v1 + b);
        common::Word qPrime{(*selectors)[hypothesis->state]};

        common::Stack x{oracle.stackContentQuery(v1)};
        common::Stack xPrime{oracle.stackContentQuery(v1 + b)};

        common::Word suffix{common::Word{xPrime} + v2};

        if (oracle.stackContentQuery(v1 + b + suffix) != oracle.stackContentQuery(qPrime + suffix))
        {
            common::symbol::StackSymbol bPrime{oracle.stackContentQuery(v1).top()}; // not needed

            testWords->addWord(suffix);
            addNewSelectorIfNeeded(v1 + b);
            return;
        }
    }
    ERR("ERROR");
    exit(1);
}

void Learner::addNewSelectorIfNeeded(const common::Word &selector)
{
    // TODO
    selectors->addSelector(selector, oracle.membershipQuery(selector));
}
} // namespace learner