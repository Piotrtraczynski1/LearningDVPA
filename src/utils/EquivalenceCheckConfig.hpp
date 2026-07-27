#pragma once

#ifdef RANDOM_EQUIVALENCE_QUERY

#include <cstdint>

namespace utils
{
constexpr int equivalenceCheckSeed{1};
constexpr uint32_t numOfRandomWords{600000000};
constexpr uint16_t maxLengthRandomWord{100};
} // namespace utils

#endif
