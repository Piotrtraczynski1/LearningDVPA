#pragma once

#include <cstdint>

#ifndef RANDOM_EQUIVALENCE_QUERY

namespace utils
{
constexpr uint16_t MaxNumOfAutomatonStates{40};
constexpr uint16_t MaxNumOfLetters{20};
constexpr uint16_t MaxNumOfStackSymbols{20};

constexpr uint16_t MaxNumOfCombinedAutomatonStates{
    (MaxNumOfAutomatonStates + 2) * (MaxNumOfAutomatonStates + 1) + 2};
constexpr uint16_t MaxNumOfCombinedAutomatonLetters{MaxNumOfLetters + 1};
constexpr uint16_t MaxNumOfCombinedStackSymbols{
    (MaxNumOfStackSymbols + 1) * (MaxNumOfStackSymbols + 1) + 1};
} // namespace utils

#else

namespace utils
{
namespace
{
constexpr uint16_t dummyValue{10};
constexpr uint16_t maxValue{0xffff};
} // namespace

constexpr uint16_t MaxNumOfAutomatonStates{500};
constexpr uint16_t MaxNumOfLetters{50};
constexpr uint16_t MaxNumOfStackSymbols{50};

constexpr uint16_t MaxNumOfCombinedAutomatonStates{maxValue};
constexpr uint16_t MaxNumOfCombinedAutomatonLetters{maxValue};
constexpr uint16_t MaxNumOfCombinedStackSymbols{maxValue};
} // namespace utils

#endif