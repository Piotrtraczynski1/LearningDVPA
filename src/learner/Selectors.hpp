#pragma once

#include <iostream>
#include <limits>

#include "common/Word.hpp"
#include "common/transition/State.hpp"
#include "utils/Constants.hpp"

namespace learner
{
// This class contains standard words
class Selectors
{
    uint16_t numOfSelectors{0};
    common::Word selectors[utils::MaxNumOfAutomatonStates] = {};
    common::transition::State states[utils::MaxNumOfAutomatonStates] = {};
    std::vector<uint16_t> acceptingWords{};

public:
    static constexpr uint16_t INVALID_INDEX{std::numeric_limits<uint16_t>::max()};
    void addSelector(common::Word s, bool accepting)
    {
        selectors[numOfSelectors] = s;
        states[numOfSelectors] = common::transition::State{numOfSelectors};
        if (accepting)
        {
            acceptingWords.push_back(numOfSelectors);
        }
        numOfSelectors++;
    }

    uint16_t size() const
    {
        return numOfSelectors;
    }

    const common::transition::State getState(uint16_t stateIndex) const
    {
        return states[stateIndex];
    }

    const common::Word &operator[](const uint16_t it) const
    {
        return selectors[it];
    }

    const std::vector<uint16_t> &getAcceptingStates()
    {
        return acceptingWords;
    }
};
} // namespace learner