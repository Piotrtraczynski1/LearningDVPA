#include <iostream>

#include "learner/Learner.hpp"
#include "utils/TimeMarker.hpp"

namespace learner
{

Learner::Learner(
    teacher::Teacher &teacher, uint16_t numOfC, uint16_t numOfR, uint16_t numOfL, uint16_t numOfS,
    Srs srs)
    : oracle{teacher}, numOfCalls{numOfC}, numOfReturns{numOfR}, numOfLocals{numOfL},
      numOfStackSymbols{numOfS}, selectors{std::make_shared<Selectors>()},
      testWords{std::make_shared<TestWords>()},
      generator{oracle,       selectors,   testWords,        numOfCalls,
                numOfReturns, numOfLocals, numOfStackSymbols},
      srsChecker{selectors, testWords, srs, teacher}
{
    setInitialState();
};

std::shared_ptr<common::VPA> Learner::run()
{
    TIME_MARKER("[Learner]: Learner::run");
    std::shared_ptr<common::Word> counterExample;

    IMP("[Learner]: Learner running");
    while (true)
    {
        hypothesis = generator.generate();

        counterExample = std::make_shared<common::Word>(srsChecker.check(hypothesis));
        if (counterExample->empty())
        {
            counterExample = oracle.equivalenceQuery(hypothesis);
        }

        if (counterExample->empty())
        {
            IMP("[Learner]: VPA found numOfStates: %u", hypothesis->getNumOfStates());
            return hypothesis;
        }

        handleCounterExample(counterExample);

        // std::cout << "========== SELECTORS =============\n";
        // for (uint16_t it = 0; it < selectors->size(); it++)
        // {
        //     std::cout << it << ": " << (*selectors)[it] << "\n";
        // }
        // std::cout << "==================================\n";
        // std::cout << "========= TEST WRODS =============\n";
        // for (uint16_t it = 0; it < testWords->size(); it++)
        // {
        //     std::cout << it << ": " << (*testWords)[it] << "\n";
        // }
        // std::cout << "==================================\n";
    }
}

void Learner::handleCounterExample(std::shared_ptr<common::Word> counterExample)
{
    TIME_MARKER("[Learner]: handleCounterExample");
    std::cout << "\033[32m[Learner]: CounterExample: " << *counterExample << "\033[0m" << std::endl;

    for (uint16_t i = 0; i < counterExample->size(); i++)
    {
        common::Word v{counterExample->begin(), counterExample->begin() + i};
        common::Word a{1, (*counterExample)[i]};
        common::Word w{counterExample->begin() + i + 1, counterExample->end()};

        hypothesis->checkWord(v + a);
        if (oracle.stackContentQuery(v + a) != hypothesis->stack)
        {
            handleStackContentDiverges(v + a, a, w);
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

    ERR("[Learner]: Counter example partition not found membershipQuery()=%d, "
        "hypothesis->checkWord()=%d",
        oracle.membershipQuery(*counterExample), hypothesis->checkWord(*counterExample));
    exit(1);
}

void Learner::handleSuffixesMismatch(
    const common::Word &v, const common::Word &a, const common::Word &w)
{
    addNewSelectorIfNeeded(v + a);
    testWords->addWord(common::Word{oracle.stackContentQuery(v + a)} + w);
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

            addNewSelectorIfNeeded(v1 + b);
            testWords->addWord(suffix);
            return;
        }
    }

    ERR("[Learner]: Word partition in handleStackContentDiverges not found!");
    exit(2);
}

void Learner::addNewSelectorIfNeeded(const common::Word &selector)
{
    generator.findOrAddSuccessor(selector);
}
} // namespace learner