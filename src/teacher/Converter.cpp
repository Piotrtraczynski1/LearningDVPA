#include "teacher/Converter.hpp"

#include <iostream>

#include "teacher/cfg/Calculator.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace teacher
{
VPA Converter::combineVPA(const VPA &secondVpa)
{
    TIME_MARKER("combineVPA");
    transition.clear();

    const uint16_t numOfStates{static_cast<uint16_t>(vpa->numOfStates * secondVpa.numOfStates)};
    const uint16_t stackSymbolsNumber{static_cast<uint16_t>(numOfStackSymbols * numOfStackSymbols)};

    std::vector<uint16_t> acceptingStates{};

    for (uint16_t state = 0; state < numOfStates; state++)
    {
        addCalls(state, secondVpa);
        addLocals(state, secondVpa);
        for (uint16_t stackSymbol = 0; stackSymbol < stackSymbolsNumber; stackSymbol++)
        {
            addReturns(state, stackSymbol, secondVpa);
        }

        if (isAcceptingState(state, secondVpa))
        {
            acceptingStates.push_back(state);
        }
    };

    return VPA{transition, common::transition::State{0}, acceptingStates, numOfStates};
}

std::shared_ptr<cfg::Cfg> Converter::convertVpaToCfg(const VPA &vpa)
{
    TIME_MARKER("convertVpaToCfg");
    cfg = std::make_shared<cfg::Cfg>();

    const uint16_t numOfStates{vpa.numOfStates};
    const uint16_t stackSymbolsNumber{static_cast<uint16_t>(numOfStackSymbols * numOfStackSymbols)};

    const uint64_t tmp{static_cast<uint64_t>(
        numOfStates * numOfStates * stackSymbolsNumber *
        (numOfCalls + numOfLocals + numOfReturns))};

    cfg = std::make_shared<cfg::Cfg>(numOfStates * tmp,
                                     tmp); // TODO Make better approximation

    for (uint16_t stateId = 0; stateId < numOfStates; stateId++)
    {
        common::transition::State state{stateId};
        for (uint16_t stackSymbolId = 0; stackSymbolId < stackSymbolsNumber; stackSymbolId++)
        {
            common::symbol::StackSymbol stackSymbol{stackSymbolId};

            addCallProjections(vpa.delta.callT[state], numOfStates, state, stackSymbol);
            addReturnProjections(
                vpa.delta.returnT[state][stackSymbol], numOfStates, state, stackSymbol);
            addLocalProjections(vpa.delta.localT[state], numOfStates, state, stackSymbol);
        }
    }

    addCommonProjections(vpa);

    return cfg;
}

void Converter::addCalls(uint16_t state, const VPA &secondVpa)
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        auto states{convertCombinedStateIntoStates(state)};
        const common::symbol::CallSymbol callSymbol{call};

        common::transition::CoArgument coArg1{vpa->delta(states.first, callSymbol)};
        common::transition::CoArgument coArg2{secondVpa.delta(states.second, callSymbol)};

        if (coArg1.state == common::transition::State::INVALID or
            coArg2.state == common::transition::State::INVALID)
        {
            continue;
        }

        transition.add(
            common::transition::State{state}, callSymbol, combineStates(coArg1.state, coArg2.state),
            combineStackSymbols(coArg1.stackSymbol, coArg2.stackSymbol));
    }
}

void Converter::addLocals(uint16_t state, const VPA &secondVpa)
{
    for (uint16_t local = 0; local < numOfLocals; local++)
    {
        auto states{convertCombinedStateIntoStates(state)};
        const common::symbol::LocalSymbol localSymbol{local};

        common::transition::State coArg1{vpa->delta(states.first, localSymbol)};
        common::transition::State coArg2{secondVpa.delta(states.second, localSymbol)};

        if (coArg1 == common::transition::State::INVALID or
            coArg2 == common::transition::State::INVALID)
        {
            continue;
        }

        transition.add(
            common::transition::State{state}, localSymbol, combineStates(coArg1, coArg2));
    }
}

void Converter::addReturns(uint16_t state, uint16_t stackSymbol, const VPA &secondVpa)
{
    for (uint16_t ret = 0; ret < numOfReturns; ret++)
    {
        auto stackSymbols{convertCombinedStackSymbolIntoSymbols(stackSymbol)};
        auto states{convertCombinedStateIntoStates(state)};

        const common::symbol::ReturnSymbol returnSymbol{ret};

        common::transition::State coArg1{
            vpa->delta(states.first, stackSymbols.first, returnSymbol)};
        common::transition::State coArg2{
            secondVpa.delta(states.second, stackSymbols.second, returnSymbol)};

        if (coArg1 == common::transition::State::INVALID or
            coArg2 == common::transition::State::INVALID)
        {
            continue;
        }

        transition.add(
            common::transition::State{state}, common::symbol::StackSymbol{stackSymbol},
            returnSymbol, combineStates(coArg1, coArg2));
    }
}

std::pair<common::transition::State, common::transition::State>
Converter::convertCombinedStateIntoStates(uint16_t state) const
{
    return {
        common::transition::State{static_cast<uint16_t>(state % vpa->numOfStates)},
        common::transition::State{static_cast<uint16_t>(state / vpa->numOfStates)}};
}

std::pair<common::symbol::StackSymbol, common::symbol::StackSymbol>
Converter::convertCombinedStackSymbolIntoSymbols(uint16_t stackSymbol) const
{
    return {
        common::symbol::StackSymbol{static_cast<uint16_t>(stackSymbol % numOfStackSymbols)},
        common::symbol::StackSymbol{static_cast<uint16_t>(stackSymbol / numOfStackSymbols)}};
}

common::transition::State Converter::combineStates(uint16_t s1, uint16_t s2) const
{
    return common::transition::State{static_cast<uint16_t>(s2 * vpa->numOfStates + s1)};
}

common::symbol::StackSymbol Converter::combineStackSymbols(uint16_t s1, uint16_t s2) const
{
    return common::symbol::StackSymbol{static_cast<uint16_t>(s2 * numOfStackSymbols + s1)};
}

bool Converter::isAcceptingState(uint16_t state, const VPA &secondVpa) const
{
    std::pair<common::transition::State, common::transition::State> states{
        convertCombinedStateIntoStates(state)};
    return vpa->acceptingStates.at(states.first) ^ secondVpa.acceptingStates.at(states.second);
}

void Converter::addCallProjections(
    const common::transition::CoArgument (&callT)[utils::MaxNumOfLetters],
    const uint16_t numOfStates, const common::transition::State state,
    const common::symbol::StackSymbol stackSymbol)
{
    for (uint16_t callId = 0; callId < numOfCalls; callId++)
    {
        common::symbol::CallSymbol callSymbol{callId};

        for (uint16_t firstStateId = 0; firstStateId < numOfStates; firstStateId++)
        {
            transition::State firstState{firstStateId};

            for (uint16_t secondStateId = 0; secondStateId < numOfStates; secondStateId++)
            {
                transition::State secondState{secondStateId};

                cfg::NonTerminal nonTerminal{
                    cfg::Calculator::makeNonTerminal(state, stackSymbol, firstState)};

                common::transition::CoArgument coArg{callT[callId]};

                cfg->addProjection(
                    nonTerminal, cfg::Calculator::convertSymbolToTerminal(callSymbol),
                    cfg::Calculator::makeNonTerminal(coArg.state, coArg.stackSymbol, secondState),
                    cfg::Calculator::makeNonTerminal(secondState, stackSymbol, firstState));
            }
        }
    }
}

void Converter::addReturnProjections(
    const common::transition::State (&returnT)[utils::MaxNumOfLetters], const uint16_t numOfStates,
    const common::transition::State state, const common::symbol::StackSymbol stackSymbol)
{
    for (uint16_t returnId = 0; returnId < numOfReturns; returnId++)
    {
        common::symbol::ReturnSymbol returnSymbol{returnId};

        if (stackSymbol == symbol::StackSymbol::BOTTOM)
        {
            for (uint16_t firstStateId = 0; firstStateId < numOfStates; firstStateId++)
            {
                transition::State firstState{firstStateId};
                cfg::NonTerminal nonTerminal{
                    cfg::Calculator::makeNonTerminal(state, stackSymbol, firstState)};

                cfg->addProjection(
                    nonTerminal, cfg::Calculator::convertSymbolToTerminal(returnSymbol),
                    cfg::Calculator::makeNonTerminal(
                        returnT[returnSymbol], stackSymbol, firstState),
                    std::nullopt);
            }
        }
        else
        {
            cfg::NonTerminal nonTerminal{
                cfg::Calculator::makeNonTerminal(state, stackSymbol, returnT[returnSymbol])};

            cfg->addProjection(
                nonTerminal, cfg::Calculator::convertSymbolToTerminal(returnSymbol), std::nullopt,
                std::nullopt);
        }
    }
}

void Converter::addLocalProjections(
    const common::transition::State (&localT)[utils::MaxNumOfLetters], const uint16_t numOfStates,
    const common::transition::State state, const common::symbol::StackSymbol stackSymbol)
{
    for (uint16_t localId = 0; localId < numOfLocals; localId++)
    {
        common::symbol::LocalSymbol localSymbol{localId};
        for (uint16_t firstStateId = 0; firstStateId < numOfStates; firstStateId++)
        {
            transition::State firstState{firstStateId};
            cfg::NonTerminal nonTerminal{
                cfg::Calculator::makeNonTerminal(state, stackSymbol, firstState)};

            cfg->addProjection(
                nonTerminal, cfg::Calculator::convertSymbolToTerminal(localSymbol),
                cfg::Calculator::makeNonTerminal(localT[localSymbol], stackSymbol, firstState),
                std::nullopt);
        }
    }
}

void Converter::addCommonProjections(const VPA &vpa)
{
    for (uint16_t stateId = 0; stateId < vpa.numOfStates; stateId++)
    {
        if (vpa.acceptingStates[stateId])
        {
            transition::State state{stateId};

            cfg->addProjection(
                cfg::NonTerminal::START, std::nullopt,
                cfg::Calculator::makeNonTerminal(
                    vpa.initialState, symbol::StackSymbol::BOTTOM, state),
                std::nullopt);

            for (uint16_t stackSymbol = 0; stackSymbol <= numOfStackSymbols * numOfStackSymbols;
                 stackSymbol++)
            {
                symbol::StackSymbol ss{stackSymbol};
                if (stackSymbol == numOfStackSymbols * numOfStackSymbols)
                {
                    ss = symbol::StackSymbol::BOTTOM;
                }
                cfg::NonTerminal nonTerminal{cfg::Calculator::makeNonTerminal(state, ss, state)};
                cfg->addProjection(nonTerminal, std::nullopt, std::nullopt, std::nullopt);
            }
        }
    }
}
} // namespace teacher
