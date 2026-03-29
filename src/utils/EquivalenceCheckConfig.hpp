#pragma once

#ifdef RANDOM_EQUIVALENCE_QUERY

#include <cstdint>

namespace utils
{
constexpr int equivalenceCheckSeed{1};
constexpr uint32_t numOfRandomWords{100000};
constexpr uint16_t maxLengthRandomWord{50};
} // namespace utils

#endif