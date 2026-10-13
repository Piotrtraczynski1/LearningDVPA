#pragma once

#include <cstdint>

#include "utils/Constants.hpp"
#include "utils/EquivalenceCheckConfig.hpp"

namespace utils
{
enum class SrsCheckMethod : uint8_t
{
    Combined,
    Mixed
};

constexpr const char *toString(const SrsCheckMethod method)
{
    switch (method)
    {
    case SrsCheckMethod::Combined:
        return "combined";
    case SrsCheckMethod::Mixed:
        return "mixed";
    }

    return "unknown";
}

struct SrsCheckConfig
{
    SrsCheckMethod method{SrsCheckMethod::Combined};
    uint16_t maxNumOfRevealedWellMatchedWords{MaxNumOfRevealedWellMatchedWordsForSrsCheck};
    uint16_t maxNumOfWellMatchedWords{MaxNumOfWellMatchedWordsForSrsCheck};
    uint32_t numOfRandomWords{numOfRandomWordsForMixedSrsCheck};
};
} // namespace utils
