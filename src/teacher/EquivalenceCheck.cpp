#include "teacher/EquivalenceCheck.hpp"
#include "utils/EquivalenceCheckConfig.hpp"

#ifndef RANDOM_EQUIVALENCE_QUERY

namespace teacher
{
std::shared_ptr<common::Word> equivalenceCheck(
    std::shared_ptr<teacher::Converter> converter,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa)
{
    auto combinedVpa{converter->combineVPA(*vpa)};

    return converter->convertVpaToCfg(*combinedVpa);
}
} // namespace teacher

#else

#include <random>

namespace teacher
{
std::shared_ptr<common::Word> equivalenceCheck(
    std::shared_ptr<teacher::Converter> converter,
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa)
{
    std::mt19937 rng(utils::equivalenceCheckSeed);

    auto generateRandomWord = [&](const uint16_t length)
    {
        common::Word word{};

        for (uint16_t i = 0; i < length; i++)
        {
            uint8_t symbol{static_cast<uint8_t>(rng() % 3)};

            switch (symbol)
            {
            case 0:
                if (converter->numOfCalls != 0)
                {
                    word += common::Word{common::symbol::CallSymbol{
                        static_cast<uint16_t>(rng() % converter->numOfCalls)}};
                }
                break;
            case 1:
                if (converter->numOfLocals != 0)
                {
                    word += common::Word{common::symbol::LocalSymbol{
                        static_cast<uint16_t>(rng() % converter->numOfLocals)}};
                }
                break;
            case 2:
                if (converter->numOfReturns != 0)
                {
                    word += common::Word{common::symbol::ReturnSymbol{
                        static_cast<uint16_t>(rng() % converter->numOfReturns)}};
                }
                break;
            default:
                break;
            }
        }

        return std::make_shared<common::Word>(word);
    };

    for (uint32_t i = 0; i < utils::numOfRandomWords; i++)
    {
        const uint16_t length{static_cast<uint16_t>(rng() % (utils::maxLengthRandomWord + 1))};
        std::shared_ptr<common::Word> testWord{generateRandomWord(length)};

        if (converter->vpa->checkWord(*testWord) != vpa->checkWord(*testWord))
        {
            return testWord;
        }
    }

    return std::make_shared<common::Word>();
}
} // namespace teacher

#endif