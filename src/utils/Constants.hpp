#pragma once

#include <cstdint>

#ifndef RANDOM_EQUIVALENCE_QUERY

namespace utils
{
constexpr uint16_t MaxNumOfAutomatonStates{42};
constexpr uint16_t MaxNumOfLetters{1000};
constexpr uint16_t MaxNumOfStackSymbols{40};

constexpr uint16_t MaxNumOfRevealedWellMatchedWordsForSrsCheck{100};
constexpr uint16_t MaxNumOfWellMatchedWordsForSrsCheck{40};

constexpr uint16_t MaxNumOfCombinedAutomatonStates{
    (MaxNumOfAutomatonStates + 2) * (MaxNumOfAutomatonStates + 1) + 2};
constexpr uint16_t MaxNumOfCombinedAutomatonLetters{MaxNumOfLetters + 1};
constexpr uint16_t MaxNumOfCombinedStackSymbols{
    (MaxNumOfStackSymbols + 1) * (MaxNumOfStackSymbols + 1) + 1};
} // namespace utils

#else // RANDOM_EQUIVALENCE_QUERY

namespace utils
{
namespace
{
constexpr uint16_t maxValue{0xffff};
} // namespace

constexpr uint16_t MaxNumOfAutomatonStates{42};
constexpr uint16_t MaxNumOfLetters{300};
constexpr uint16_t MaxNumOfStackSymbols{40};

constexpr uint16_t MaxNumOfRevealedWellMatchedWordsForSrsCheck{5000};
constexpr uint16_t MaxNumOfWellMatchedWordsForSrsCheck{200};

constexpr uint16_t MaxNumOfCombinedAutomatonStates{
    (MaxNumOfAutomatonStates + 2) * (MaxNumOfAutomatonStates + 1) + 2};
constexpr uint16_t MaxNumOfCombinedAutomatonLetters{MaxNumOfLetters + 1};
constexpr uint16_t MaxNumOfCombinedStackSymbols{
    (MaxNumOfStackSymbols + 1) * (MaxNumOfStackSymbols + 1) + 1};
} // namespace utils

#endif // RANDOM_EQUIVALENCE_QUERY