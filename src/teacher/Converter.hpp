#pragma once

#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"
#include "teacher/cfg/Cfg.hpp"

using namespace common;

namespace teacher
{
class Converter
{
    const std::shared_ptr<VPA> vpa;
    std::shared_ptr<cfg::Cfg> cfg;

public:
    Converter(
        const std::shared_ptr<VPA> vpa, uint16_t numCalls, uint16_t numReturns, uint16_t numLocals,
        uint16_t stackSymbolsNumber)
        : vpa{vpa}, numOfCalls{numCalls}, numOfReturns{numReturns}, numOfLocals{numLocals},
          numOfStackSymbols{stackSymbolsNumber} {};

    VPA combineVPA(const VPA &secondVpa);
    std::shared_ptr<cfg::Cfg> convertVpaToCfg(const VPA &vpa);

private:
    common::transition::Transition transition;

    void addCalls(uint16_t state, const VPA &secondVpa);
    void addLocals(uint16_t state, const VPA &secondVpa);
    void addReturns(uint16_t state, uint16_t stackSymbol, const VPA &secondVpa);
    bool isAcceptingState(uint16_t state, const VPA &secondVpa) const;

    common::symbol::StackSymbol combineStackSymbols(uint16_t s1, uint16_t s2) const;
    common::transition::State combineStates(uint16_t s1, uint16_t s2) const;
    std::pair<common::symbol::StackSymbol, common::symbol::StackSymbol>
    convertCombinedStackSymbolIntoSymbols(uint16_t stackSymbol) const;
    std::pair<common::transition::State, common::transition::State>
    convertCombinedStateIntoStates(uint16_t state) const;

    void addCallProjections(
        const common::transition::CoArgument (&callT)[utils::MaxNumOfLetters],
        const uint16_t numOfStates, const common::transition::State state,
        const common::symbol::StackSymbol stackSymbol);
    void addReturnProjections(
        const common::transition::State (&returnT)[utils::MaxNumOfLetters],
        const uint16_t numOfStates, const common::transition::State state,
        const common::symbol::StackSymbol stackSymbol);
    void addLocalProjections(
        const common::transition::State (&localT)[utils::MaxNumOfLetters],
        const uint16_t numOfStates, const common::transition::State state,
        const common::symbol::StackSymbol stackSymbol);
    void addCommonProjections(const VPA &vpa);

    uint16_t numOfCalls;
    uint16_t numOfReturns;
    uint16_t numOfLocals;
    uint16_t numOfStackSymbols;
};
} // namespace teacher
