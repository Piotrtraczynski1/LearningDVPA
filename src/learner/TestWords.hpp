#pragma once

#include "common/Word.hpp"
#include "utils/Constants.hpp"

namespace learner
{
// This class contains single-reset words
class TestWords
{
    uint16_t numOfWords{1};
    common::Word testWords[utils::MaxNumOfCombinedAutomatonStates] = {
        common::Word{common::Stack{}}};

public:
    void addWord(common::Word s)
    {
        testWords[numOfWords] = s;
        numOfWords++;
    }

    uint16_t size() const
    {
        return numOfWords;
    }

    const common::Word &operator[](const uint16_t it) const
    {
        return testWords[it];
    }
};
} // namespace learner