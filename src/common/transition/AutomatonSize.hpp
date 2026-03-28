#pragma once

#include "utils/Constants.hpp"
#include <cstdint>

enum class AutomatonKind
{
    Normal,
    Combined
};

template <AutomatonKind Kind>
struct AutomatonSize;

template <>
struct AutomatonSize<AutomatonKind::Normal>
{
    static constexpr uint16_t MaxNumOfStates = utils::MaxNumOfAutomatonStates + 2;
    static constexpr uint16_t MaxNumOfLetters = utils::MaxNumOfLetters + 2;
    static constexpr uint16_t MaxNumOfStackSymbols = utils::MaxNumOfStackSymbols + 2;
};

template <>
struct AutomatonSize<AutomatonKind::Combined>
{
    static constexpr uint16_t MaxNumOfStates = utils::MaxNumOfCombinedAutomatonStates;
    static constexpr uint16_t MaxNumOfLetters = utils::MaxNumOfCombinedAutomatonLetters;
    static constexpr uint16_t MaxNumOfStackSymbols = utils::MaxNumOfCombinedStackSymbols;
};