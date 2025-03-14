#pragma once

#include <cstdint>
#include <memory>
#include <vector>

// move with constructor to cpp
#include <ctime>
#include <random>

#include "common/Stack.hpp"
#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"
#include "common/transition/Transition.hpp"

namespace generator
{
class Generator
{
    const uint16_t numOfStates;
    const uint16_t numOfCalls;
    const uint16_t numOfLocals;
    const uint16_t numOfReturns;
    const uint16_t numOfStackSymbols;
    const uint16_t parameter;

    // To be removed, when VPA holds pointer to transition
    std::shared_ptr<common::transition::Transition> transition;

public:
    Generator(
        const uint16_t numOfStates_, const uint16_t numOfCalls_, const uint16_t numOfLocals_,
        const uint16_t numOfReturns_, const uint16_t numOfStackSymbols_,
        const uint16_t parameter_ = 1)
        : numOfStates{numOfStates_}, numOfCalls{numOfCalls_}, numOfLocals{numOfLocals_},
          numOfReturns{numOfReturns_}, numOfStackSymbols{numOfStackSymbols_}, parameter{parameter_}
    {
        transition = std::make_shared<common::transition::Transition>();
        std::srand(std::time(nullptr));
    };

    std::shared_ptr<common::VPA> run();

    void generateTransition();
    std::vector<uint16_t> selectAcceptingStates();

    void addCalls(common::transition::State state);
    void addLocals(common::transition::State state);
    void addReturns(common::transition::State state);
};
} // namespace generator
