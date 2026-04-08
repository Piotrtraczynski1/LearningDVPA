#pragma once

#include <cstdint>
#include <queue>
#include <tuple>

#include "common/VPA.hpp"
#include "teacher/NonTerminal.hpp"
#include "teacher/ParentStateAndSymbol.hpp"
#include "teacher/Witness.hpp"

namespace teacher
{
class EmptinessChecker
{
    std::queue<NonTerminal> generativeNonTerminals;

    std::vector<bool> generative;
    std::vector<Witness> witnesses;

    std::vector<std::vector<ParentStateAndSymbol>> localParents;
    std::vector<std::vector<ParentStateAndSymbol>> returnParents;
    std::vector<std::vector<ParentStateAndSymbol>> callParents;
    std::vector<std::vector<NonTerminal>> generativeByLeftState;
    std::vector<std::vector<NonTerminal>> generativeByRightState;

public:
    EmptinessChecker(uint16_t numCallsArg, uint16_t numReturnsArg, uint16_t numLocalsArg);

    common::Word check(
        const std::shared_ptr<common::VPA<AutomatonKind::Combined>> &vpa,
        const uint16_t stackSymbolsNumber);

private:
    void init(
        const std::shared_ptr<common::VPA<AutomatonKind::Combined>> &vpa,
        const uint16_t stackSymbolsNumber);

    void initParents(const common::VPA<AutomatonKind::Combined> &vpa);

    void addNonTerminalIfNeeded(
        const uint16_t leftState, const uint16_t stackSymbol, const uint16_t rightState,
        const Witness witness);

    void addInitialGenerativeNonTerminals(
        const std::shared_ptr<common::VPA<AutomatonKind::Combined>> &vpa);
    void addGenerativeLocals(const NonTerminal generative);
    void addGenerativeReturns(const NonTerminal generative);
    void addGenerativeCalls(const NonTerminal generative);

    void buildExample(const NonTerminal nonTerminal, common::Word &example);

    NonTerminal encodeNonTerminal(
        const uint16_t leftState, const uint16_t stackSymbol, const uint16_t rightState);
    std::tuple<uint16_t, uint16_t, uint16_t> decodeNonTerminal(const NonTerminal nonTermial);

    uint16_t numOfCalls;
    uint16_t numOfReturns;
    uint16_t numOfLocals;
    uint16_t numOfStackSymbols;
    uint16_t numOfStates;
};
} // namespace teacher