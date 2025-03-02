#pragma once

#include "../common/Word.hpp"
#include "../common/transition/State.hpp"
#include "../utils/Constants.hpp"

namespace learner
{
// This class hold standard letters
class Selectors
{
    uint16_t numOfSelectors{1};
    common::Word selectors[utils::MaxWordLength];
    common::transition::State states[utils::MaxWordLength] = {common::transition::State{0, true}};

public:
    void addSelector(common::Word s, bool isAccepting)
    {
        selectors[numOfSelectors] = s;
        states[numOfSelectors] = common::transition::State{numOfSelectors, isAccepting};
        numOfSelectors++;
    }

    uint16_t size() const { return numOfSelectors; }
    common::transition::State getState(uint16_t stateIndex) { return states[stateIndex]; }
    common::Word &operator[](uint16_t it) { return selectors[it]; }
};
} // namespace learner