#pragma once

#include <cstdint>
#include <memory>

#include "common/VPA.hpp"
#include "common/symbol/Symbols.hpp"
#include "common/transition/State.hpp"
#include "common/transition/Transition.hpp"

namespace teacher
{
template <AutomatonKind Kind = AutomatonKind::Normal>
class AutomataCombiner
{
    const uint16_t numOfCalls;
    const uint16_t numOfReturns;
    const uint16_t numOfLocals;
    const uint16_t numOfStackSymbols;

    uint16_t combinedVpaNumOfStates;
    uint16_t combinedVpaNumOfStackSymbols;

    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa;

    std::unique_ptr<common::transition::Transition<Kind>> transition;

public:
    AutomataCombiner(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &vpa, uint16_t numCalls,
        uint16_t numReturns, uint16_t numLocals, uint16_t stackSymbolsNumber);

    std::unique_ptr<common::VPA<Kind>> combineVPA(
        const common::VPA<AutomatonKind::Normal> &secondVpa);

private:
    void addCalls(uint16_t state, const common::VPA<AutomatonKind::Normal> &secondVpa);
    void addLocals(uint16_t state, const common::VPA<AutomatonKind::Normal> &secondVpa);
    void addReturns(
        uint16_t state, uint16_t stackSymbol, const common::VPA<AutomatonKind::Normal> &secondVpa);

    bool isAcceptingState(
        uint16_t state, const common::VPA<AutomatonKind::Normal> &secondVpa) const;

    common::symbol::StackSymbol combineStackSymbols(uint16_t s1, uint16_t s2) const;
    common::transition::State combineStates(uint16_t s1, uint16_t s2) const;
    std::pair<common::symbol::StackSymbol, common::symbol::StackSymbol>
    convertCombinedStackSymbolIntoSymbols(uint16_t stackSymbol) const;
    std::pair<common::transition::State, common::transition::State> convertCombinedStateIntoStates(
        uint16_t state) const;
};
} // namespace teacher