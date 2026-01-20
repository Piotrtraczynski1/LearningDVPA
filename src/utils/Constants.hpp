#pragma once

#include <cstdint>

namespace utils
{
constexpr uint16_t MaxNumOfAutomatonStates{20};
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