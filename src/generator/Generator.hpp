#pragma once

#include <cstdint>
#include <memory>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"
#include "learner/Srs.hpp"

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
    double acceptingStatesDensity;

    uint16_t numOfModules;

    std::shared_ptr<common::transition::Transition> transition;

public:
    virtual ~Generator() = default;
    Generator() = default;

    void setConfig(
        const uint16_t numOfStates_, const uint16_t numOfCalls_, const uint16_t numOfLocals_,
        const uint16_t numOfReturns_, const uint16_t numOfStackSymbols_, const double density_,
        const double acceptingStatesDensity_, const uint16_t numOfModules_)
    {
        numOfStates = numOfStates_;
        numOfCalls = numOfCalls_;
        numOfLocals = numOfLocals_;
        numOfReturns = numOfReturns_;
        numOfStackSymbols = numOfStackSymbols_;
        density = density_;
        acceptingStatesDensity = acceptingStatesDensity_;
        numOfModules = numOfModules_;

        transition = std::make_shared<common::transition::Transition>();
    }

    virtual std::shared_ptr<common::VPA> run() = 0;
    virtual bool generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis)
    {
        return true;
    }

    virtual learner::Srs getSrs()
    {
        return learner::Srs{};
    }

    bool skipTransition()
    {
        return ((static_cast<double>(rand()) / RAND_MAX) >= density);
    }

    bool shouldAccept()
    {
        return ((static_cast<double>(rand()) / RAND_MAX) >= acceptingStatesDensity);
    }
};
} // namespace generator
