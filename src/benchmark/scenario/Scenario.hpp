#pragma once

#include <cstdint>

namespace benchmark::scenario
{
class Scenario
{
public:
    virtual int getSeed() = 0;
    virtual uint16_t getNumOfIterations() = 0;
    virtual uint16_t getNumOfTestsInSingleIteration() = 0;
    virtual void runSingleIteration() = 0;
};
} // namespace benchmark::scenario