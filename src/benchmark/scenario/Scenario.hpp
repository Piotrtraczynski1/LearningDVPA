#pragma once

#include <cstdint>
#include <string>

namespace benchmark::scenario
{
class Scenario
{
public:
    virtual int getSeed() = 0;
    virtual uint16_t getNumOfIterationsIn1Dim() = 0;
    virtual uint16_t getNumOfIterationsIn2Dim()
    {
        return 1;
    }
    virtual uint16_t getNumOfIterationsIn3Dim()
    {
        return 1;
    }
    virtual uint16_t getNumOfIterationsIn4Dim()
    {
        return 1;
    }
    virtual std::string getDim1Name() = 0;
    virtual std::string getDim2Name()
    {
        return "not applicable";
    }
    virtual std::string getDim3Name()
    {
        return "not applicable";
    }
    virtual std::string getDim4Name()
    {
        return "not applicable";
    }

    virtual uint16_t getDim1Details() = 0;
    virtual uint16_t getDim2Details()
    {
        return 0;
    }
    virtual uint16_t getDim3Details()
    {
        return 0;
    }
    virtual uint16_t getDim4Details()
    {
        return 0;
    }

    virtual void prepareNextIterationDim1() = 0;
    virtual void prepareNextIterationDim2()
    {
    }
    virtual void prepareNextIterationDim3()
    {
    }
    virtual void prepareNextIterationDim4()
    {
    }
    virtual void resetDim1()
    {
    }
    virtual void resetDim2()
    {
    }
    virtual void resetDim3()
    {
    }
    virtual void resetDim4()
    {
    }

    virtual uint16_t getNumOfTestsInSingleIteration() = 0;
    virtual void runSingleIteration() = 0;
};
} // namespace benchmark::scenario