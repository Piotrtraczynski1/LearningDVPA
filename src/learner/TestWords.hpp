#pragma once

#include "../common/Word.hpp"
#include "../utils/Constants.hpp"

namespace learner
{
// This class holds single-reset words
class TestWords
{
    uint16_t numOfWords{1};
    common::Word testWords[utils::MaxWordLength];

public:
    void addWord(common::Word s)
    {
        testWords[numOfWords] = s;
        numOfWords++;
    }

    uint16_t size() const { return numOfWords; }

    common::Word &operator[](uint16_t it) { return testWords[it]; }
};
} // namespace learner