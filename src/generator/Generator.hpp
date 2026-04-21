#pragma once

#include <cstdint>
#include <memory>
#include <random>

#include "common/VPA.hpp"
#include "common/transition/State.hpp"
#include "common/transition/Transition.hpp"
#include "learner/srs/Srs.hpp"

namespace generator
{
class Generator
{
public:
    virtual ~Generator() = default;
    Generator() = default;

    void setConfig(
        uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_,
        uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_, const double density_,
        const double acceptingStatesDensity_, const uint16_t numOfModules_,
        const uint16_t secondDvpaNumOfStates_);

    virtual std::shared_ptr<common::VPA<AutomatonKind::Normal>> run() = 0;
    virtual bool generatorSpecificCheck(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis);
    virtual learner::srs::Srs getSrs();

    void setSeed(uint32_t seed);

protected:
    virtual void selectAcceptingStates();
    virtual void validateGeneratorConfig(
        uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_,
        uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_);

    bool skipTransition();
    bool shouldAccept();

    uint16_t numOfStates;
    uint16_t numOfCalls;
    uint16_t numOfLocals;
    uint16_t numOfReturns;
    uint16_t numOfStackSymbols;
    double density;
    double acceptingStatesDensity;

    uint16_t numOfModules;
    uint16_t secondDvpaNumOfStates;

    std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> transition;

    common::transition::State initialState{0};
    std::vector<uint16_t> acceptingStates{0};

    mutable std::mt19937 rng{1};
};
} // namespace generator
