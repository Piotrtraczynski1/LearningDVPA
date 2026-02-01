#include <iostream>

#include "learner/Learner.hpp"
#include "utils/TimeMarker.hpp"

#define DEBUG_LEARNER

namespace learner
{

Learner::Learner(
    teacher::Teacher &teacher, uint16_t numOfC, uint16_t numOfR, uint16_t numOfL, uint16_t numOfS,
    Srs srs)
    : oracle{teacher}, numOfCalls{numOfC}, numOfReturns{numOfR}, numOfLocals{numOfL},
      numOfStackSymbols{numOfS}, selectors{std::make_shared<Selectors>()},
      testWords{std::make_shared<TestWords>()}, calculator{oracle, selectors, testWords},
      generator{oracle,     calculator,   undefinedTransitions, selectors,        testWords,
                numOfCalls, numOfReturns, numOfLocals,          numOfStackSymbols},
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

#ifdef DEBUG_LEARNER
        std::cout << "========== HYPOTHESIS =============\n";
        hypothesis->printUt();
        std::cout << "==================================\n";
        std::cout << "========== SELECTORS =============\n";
        for (uint16_t it = 0; it < selectors->size(); it++)
        {
            std::cout << it << ": " << (*selectors)[it] << "\n";
        }
        std::cout << "==================================\n";
        std::cout << "========= TEST WRODS =============\n";
        for (uint16_t it = 0; it < testWords->size(); it++)
        {
            std::cout << it << ": " << (*testWords)[it] << "\n";
        }
        std::cout << "==================================\n";
        std::cout << "====== ADDITIONAL SELECTORS ======\n";
        const auto &additionalSelectors{selectors->getAdditionalSelectors()};
        for (uint16_t it = 0; it < additionalSelectors.size(); it++)
        {
            std::cout << it << ": " << additionalSelectors[it].first << ", "
                      << additionalSelectors[it].second << "\n";
        }
        std::cout << "==================================\n";
        std::cout << "====== UNDEFINED TRANSITIONS ======\n";
        for (const auto &tr : undefinedTransitions)
        {
            std::cout << "undef: selector: " << tr.state << ", stack: " << tr.stackSymbol
                      << ", ret: " << tr.returnSymbol << std::endl;
        }
        std::cout << "==================================\n";
#endif

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
    }
}

bool Learner::areAllTransitionsDefined(std::shared_ptr<common::Word> counterExample)
{
    for (size_t it = 0; it < counterExample->size(); it++)
    {
        if ((*counterExample)[it].index() == 1)
        {
            common::Word prefix{counterExample->begin(), counterExample->begin() + it};
            common::Stack stack{oracle.stackContentQuery(prefix)};
            hypothesis->checkWord(prefix);
            UndefinedTransition transition{
                .state = hypothesis->state,
                .stackSymbol = stack.top(),
                .returnSymbol = std::get<common::symbol::ReturnSymbol>((*counterExample)[it])};
            if (undefinedTransitions.find(transition) != undefinedTransitions.end())
            {
                std::cout << "Adding additional selector: " << prefix << "{" << transition.state
                          << ", " << transition.stackSymbol << ", " << transition.returnSymbol
                          << "}" << std::endl;

                selectors->addAdditionalSelector(prefix, stack.top());
                // testWords->addWord(
                //     common::Word{stack} +
                //     common::Word{counterExample->begin() + it, counterExample->end()});

                // addNewSelectorIfNeeded(prefix);
                return false;
            }
        }
    }
    return true;
}

void Learner::handleCounterExample(std::shared_ptr<common::Word> counterExample)
{
    TIME_MARKER("[Learner]: handleCounterExample");
    std::cout << "\033[32m[Learner]: CounterExample: " << *counterExample << "\033[0m" << std::endl;

    if (not areAllTransitionsDefined(counterExample))
    {
        WRN("Not all transitions are definded");
        return;
    }

    for (uint16_t i = 0; i < counterExample->size(); i++)
    {
        common::Word v{counterExample->begin(), counterExample->begin() + i};
        common::Word a{1, (*counterExample)[i]};
        common::Word w{counterExample->begin() + i + 1, counterExample->end()};

        // if (a[0].index() == 1)
        // {
        //     common::symbol::StackSymbol top{oracle.stackContentQuery(v).top()};
        //     hypothesis->checkWord(v);
        //     UndefinedTransition transition{
        //         .state = hypothesis->state,
        //         .stackSymbol = top,
        //         .returnSymbol = std::get<common::symbol::ReturnSymbol>(a[0])};
        //     if (undefinedTransitions.find(transition) != undefinedTransitions.end())
        //     {
        //         std::cout << "Adding additional selector: " << v << "{" << transition.state << ",
        //         "
        //                   << transition.stackSymbol << ", " << transition.returnSymbol << "}"
        //                   << std::endl;

        //         addNewSelectorIfNeeded(v);
        //         selectors->addAdditionalSelector(v, top);
        //         return;
        //     }
        // }

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
    std::cout << "handleSuffixesMismatch v=" << v << ", a=" << a << ", w=" << w << std::endl;
    testWords->addWord(common::Word{oracle.stackContentQuery(v + a)} + w);

    addNewSelectorIfNeeded(v + a);
    // addNewAndSetForcedSelectorIfNeeded(v, a);
}

void Learner::handleStackContentDiverges(
    const common::Word &v, const common::Word &a, const common::Word &w)
{
    std::cout << "handleStackContentDiverges v=" << v << ", a=" << a << ", w=" << w << std::endl;
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

    ERR("[Learner]: Word partition in handleStackContentDiverges not found!");
    exit(2);
}

void Learner::addNewAndSetForcedSelectorIfNeeded(const common::Word &v, const common::Word &a)
{
    const auto successor{addNewSelectorIfNeeded(v + a)};
    if (a[0].index() == 1 /*and successor == selectors->size() - 1*/)
    {
        selectors->addAdditionalSelector(v, oracle.stackContentQuery(v).top());
    }
}

uint16_t Learner::addNewSelectorIfNeeded(const common::Word &selector)
{
    return calculator.findOrAddSuccessor(selector);
}
} // namespace learner