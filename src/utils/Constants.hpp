#pragma once

#include <cstdint>

namespace utils
{
constexpr uint16_t MaxNumOfAutomatonStates{30};
constexpr uint16_t MaxNumOfLetters{10};
constexpr uint16_t MaxNumOfStackSymbols{15};

constexpr uint16_t MaxNumOfCombinedAutomatonStates{
    (MaxNumOfAutomatonStates + 1) * (MaxNumOfAutomatonStates + 1) + 2};
constexpr uint16_t MaxNumOfCombinedAutomatonLetters{MaxNumOfLetters + 1};
constexpr uint16_t MaxNumOfCombinedStackSymbols{
    (MaxNumOfStackSymbols + 1) * (MaxNumOfStackSymbols + 1) + 1};
} // namespace utils