#pragma once

#include <cstdint>

namespace utils
{
constexpr int equivalenceCheckSeed{1};
constexpr uint32_t numOfRandomWordsForMixedSrsCheck{10000};
constexpr uint16_t maxLengthRandomWord{100};

#ifdef RANDOM_EQUIVALENCE_QUERY
constexpr uint32_t numOfRandomWordsForMainEquivalenceCheck{3000000000};
#endif
} // namespace utils
