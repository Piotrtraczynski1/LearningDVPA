#include "teacher/EquivalenceCheck.hpp"
#include "utils/EquivalenceCheckConfig.hpp"

#if !defined(RANDOM_EQUIVALENCE_QUERY) && !defined(MIXED_EQUIVALENCE_QUERY)

namespace teacher
{
std::shared_ptr<common::Word> equivalenceCheck(
    std::shared_ptr<teacher::AutomataCombiner<AutomatonKind::Combined>> automataCombiner,
    std::shared_ptr<teacher::EmptinessChecker> emptinessChecker,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa,
    [[maybe_unused]] const bool updateCounters)
{
    auto combinedVpa{automataCombiner->combineVPA(*vpa)};

    return emptinessChecker->check(combinedVpa);
}
} // namespace teacher

#else

#include "utils/Counters.hpp"
#include <random>

namespace teacher
{
namespace
{
std::mt19937 &randomEquivalenceCheckGenerator()
{
    static std::mt19937 rng(utils::equivalenceCheckSeed);
    return rng;
}
} // namespace

#ifdef RANDOM_EQUIVALENCE_QUERY
void resetRandomEquivalenceCheckGenerator()
{
    randomEquivalenceCheckGenerator() = std::mt19937(utils::equivalenceCheckSeed);
}
#endif

std::shared_ptr<common::Word> randomEquivalenceCheck(
    std::shared_ptr<teacher::AutomataCombiner<AutomatonKind::Combined>> automataCombiner,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa, const uint32_t numOfRandomWords,
    const uint16_t maxLengthRandomWord, const bool updateCounters)
{
    auto &rng = randomEquivalenceCheckGenerator();

    auto generateRandomWord = [&](const uint16_t length)
    {
        common::Word word{};

        for (uint16_t i = 0; i < length; i++)
        {
            uint8_t symbol{static_cast<uint8_t>(rng() % 3)};

            switch (symbol)
            {
            case 0:
                if (automataCombiner->numOfCalls != 0)
                {
                    word += common::Word{common::symbol::CallSymbol{
                        static_cast<uint16_t>(rng() % automataCombiner->numOfCalls)}};
                }
                break;
            case 1:
                if (automataCombiner->numOfLocals != 0)
                {
                    word += common::Word{common::symbol::LocalSymbol{
                        static_cast<uint16_t>(rng() % automataCombiner->numOfLocals)}};
                }
                break;
            case 2:
                if (automataCombiner->numOfReturns != 0)
                {
                    word += common::Word{common::symbol::ReturnSymbol{
                        static_cast<uint16_t>(rng() % automataCombiner->numOfReturns)}};
                }
                break;
            default:
                break;
            }
        }

        return std::make_shared<common::Word>(word);
    };

    for (uint32_t i = 0; i < numOfRandomWords; i++)
    {
        const uint16_t length{static_cast<uint16_t>(rng() % (maxLengthRandomWord + 1))};
        std::shared_ptr<common::Word> testWord{generateRandomWord(length)};

        if (updateCounters)
        {
            Counters::update("equivalenceQueryTargetMembershipQuery", 1);
            Counters::update("equivalenceQueryTargetMembershipQuerySymbols", testWord->size());
        }

        const bool targetAccepts{automataCombiner->vpa->checkWord(*testWord)};

        if (targetAccepts != vpa->checkWord(*testWord))
        {
            return testWord;
        }
    }

    return std::make_shared<common::Word>();
}

std::shared_ptr<common::Word> equivalenceCheck(
    std::shared_ptr<teacher::AutomataCombiner<AutomatonKind::Combined>> automataCombiner,
    std::shared_ptr<teacher::EmptinessChecker> emptinessChecker,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa, const bool updateCounters)
{
#ifdef RANDOM_EQUIVALENCE_QUERY
    return randomEquivalenceCheck(
        automataCombiner, vpa, utils::numOfRandomWordsForMainEquivalenceCheck,
        utils::maxLengthRandomWord, updateCounters);
#else
    [[maybe_unused]] const auto ignoredUpdateCounters{updateCounters};
    auto combinedVpa{automataCombiner->combineVPA(*vpa)};
    return emptinessChecker->check(combinedVpa);
#endif
}
} // namespace teacher

#endif
