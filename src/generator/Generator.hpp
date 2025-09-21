#pragma once

#include <cstdint>
#include <memory>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"

namespace generator
{
class Generator
{
protected:
    uint16_t numOfStates;
    uint16_t numOfCalls;
    uint16_t numOfLocals;
    uint16_t numOfReturns;
    uint16_t numOfStackSymbols;
    double density;

    std::shared_ptr<common::transition::Transition> transition;

public:
    virtual ~Generator() = default;
    Generator() = default;

    void setConfig(
        const uint16_t numOfStates_, const uint16_t numOfCalls_, const uint16_t numOfLocals_,
        const uint16_t numOfReturns_, const uint16_t numOfStackSymbols_, const double density_)
    {
        numOfStates = numOfStates_;
        numOfCalls = numOfCalls_;
        numOfLocals = numOfLocals_;
        numOfReturns = numOfReturns_;
        numOfStackSymbols = numOfStackSymbols_;
        density = density_;

        transition = std::make_shared<common::transition::Transition>();
    }

    virtual std::shared_ptr<common::VPA> run() = 0;
    virtual bool generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis)
    {
        return true;
    }
};
} // namespace generator
