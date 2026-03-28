#pragma once

#include <bitset>
#include <memory>
#include <queue>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"
#include "teacher/cfg/Cfg.hpp"
#include "utils/Constants.hpp"

using namespace common;

namespace teacher
{
class Converter
{
    const std::shared_ptr<VPA<AutomatonKind::Normal>> vpa;
    std::shared_ptr<cfg::Cfg> cfg;

public:
    Converter(
        const std::shared_ptr<VPA<AutomatonKind::Normal>> vpa, uint16_t numCalls,
        uint16_t numReturns, uint16_t numLocals, uint16_t stackSymbolsNumber)
        : vpa{vpa}, numOfCalls{numCalls}, numOfReturns{numReturns}, numOfLocals{numLocals},
          numOfStackSymbols{stackSymbolsNumber} {};

    VPA<AutomatonKind::Combined> combineVPA(const VPA<AutomatonKind::Normal> &secondVpa);
    std::shared_ptr<cfg::Cfg> convertVpaToCfg(const VPA<AutomatonKind::Combined> &vpa);

private:
    std::unique_ptr<common::transition::Transition<AutomatonKind::Combined>> transition;

    void addCalls(uint16_t state, const VPA<AutomatonKind::Normal> &secondVpa);
    void addLocals(uint16_t state, const VPA<AutomatonKind::Normal> &secondVpa);
    void
    addReturns(uint16_t state, uint16_t stackSymbol, const VPA<AutomatonKind::Normal> &secondVpa);
    bool isAcceptingState(uint16_t state, const VPA<AutomatonKind::Normal> &secondVpa) const;

    common::symbol::StackSymbol combineStackSymbols(uint16_t s1, uint16_t s2) const;
    common::transition::State combineStates(uint16_t s1, uint16_t s2) const;
    std::pair<common::symbol::StackSymbol, common::symbol::StackSymbol>
    convertCombinedStackSymbolIntoSymbols(uint16_t stackSymbol) const;
    std::pair<common::transition::State, common::transition::State>
    convertCombinedStateIntoStates(uint16_t state) const;

    void insertNonTerminalIfNeeded(const cfg::NonTerminal nonTerminal);
    std::tuple<size_t, size_t> calculateEstimatedCfgSize();

    void addCommonProjections(const VPA<AutomatonKind::Combined> &vpa);
    void addCallProjections(
        const common::transition::CoArgument (&callT)[utils::MaxNumOfCombinedAutomatonLetters],
        const cfg::NonTerminal nonTerminal, const common::symbol::StackSymbol stackSymbol,
        const common::transition::State state2);
    void addReturnProjections(
        const common::transition::State (&returnT)[utils::MaxNumOfCombinedAutomatonLetters],
        const cfg::NonTerminal nonTerminal, const common::symbol::StackSymbol stackSymbol,
        const common::transition::State state2);
    void addLocalProjections(
        const common::transition::State (&localT)[utils::MaxNumOfCombinedAutomatonLetters],
        const cfg::NonTerminal nonTerminal, const common::symbol::StackSymbol stackSymbol,
        const common::transition::State state2);

    uint16_t numOfCalls;
    uint16_t numOfReturns;
    uint16_t numOfLocals;
    uint16_t numOfStackSymbols;

    uint16_t combinedVpaNumOfStates;
    uint16_t combinedVpaNumOfStackSymbols;

    std::bitset<utils::MaxNumOfNonTerminals> seenNonTerminals{};
    std::queue<cfg::NonTerminal> nonTerminalsQueue{};
};
} // namespace teacher
