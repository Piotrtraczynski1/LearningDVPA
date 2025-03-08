#pragma once

#include "common/Word.hpp"
#include "common/transition/State.hpp"
#include "utils/Constants.hpp"

namespace learner
{
// This class contains standard words
class Selectors
{
    uint16_t numOfSelectors{1};
    common::Word selectors[utils::MaxNumOfAutomataStates];
    common::transition::State states[utils::MaxNumOfAutomataStates] = {
        common::transition::State{0, true}};

public:
    void addSelector(common::Word s, bool isAccepting)
    {
        selectors[numOfSelectors] = s;
        states[numOfSelectors] = common::transition::State{numOfSelectors, isAccepting};
        numOfSelectors++;
    }

    uint16_t size() const { return numOfSelectors; }
    const common::transition::State getState(uint16_t stateIndex) const
    {
        return states[stateIndex];
    }
    const common::Word &operator[](const uint16_t it) const { return selectors[it]; }
};
} // namespace learner