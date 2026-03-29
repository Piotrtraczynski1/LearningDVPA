#pragma once

#include <cstdint>

#ifndef RANDOM_EQUIVALENCE_QUERY

namespace utils
{
constexpr uint16_t MaxNumOfAutomatonStates{15};
constexpr uint16_t MaxNumOfLetters{10};
constexpr uint16_t MaxNumOfStackSymbols{10};

constexpr uint16_t MaxNumOfCombinedAutomatonStates{
    (MaxNumOfAutomatonStates + 2) * (MaxNumOfAutomatonStates + 1) + 2};
constexpr uint16_t MaxNumOfCombinedAutomatonLetters{MaxNumOfLetters + 1};
constexpr uint16_t MaxNumOfCombinedStackSymbols{
    (MaxNumOfStackSymbols + 1) * (MaxNumOfStackSymbols + 1) + 1};

constexpr uint16_t Q{MaxNumOfCombinedAutomatonStates + 1};
constexpr uint16_t S{MaxNumOfCombinedStackSymbols + 2};

constexpr uint32_t MaxNumOfNonTerminals{Q * Q * S};
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

constexpr uint16_t Q{dummyValue};
constexpr uint16_t S{dummyValue};

constexpr uint32_t MaxNumOfNonTerminals{dummyValue};
} // namespace utils

#endif