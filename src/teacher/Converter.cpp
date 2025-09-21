#include "teacher/Converter.hpp"

#include <cassert>
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

    combinedVpaNumOfStates =
        static_cast<uint16_t>((vpa->numOfStates + 1) * (secondVpa.numOfStates + 1)); // +1 for sink
    combinedVpaNumOfStackSymbols = static_cast<uint16_t>(
        (numOfStackSymbols + 1) *
        (numOfStackSymbols + 1)); // + 1 for bottom (numOfStackSymbols - 1 (Bottom) + 1 (Sink))

    LOG("[Converter]: combineVpa: numOfStates: %u, stackSymbolsNumber: %u",
        combinedVpaNumOfStates + 1, combinedVpaNumOfStackSymbols);

    std::vector<uint16_t> acceptingStates{};

    for (uint16_t state = 0; state < combinedVpaNumOfStates; state++)
    {
        addCalls(state, secondVpa);
        addLocals(state, secondVpa);
        for (uint16_t stackSymbol = 0; stackSymbol < combinedVpaNumOfStackSymbols; stackSymbol++)
        {
            addReturns(state, stackSymbol, secondVpa);
        }

        if (isAcceptingState(state, secondVpa))
        {
            acceptingStates.push_back(state);
        }
    };

    acceptingStates.push_back(combinedVpaNumOfStates);
    common::symbol::ReturnSymbol popStack{numOfReturns};
    common::transition::State popStackState{combinedVpaNumOfStates};
    for (const auto state : acceptingStates)
    {
        for (uint16_t stackSymbol = 0; stackSymbol < combinedVpaNumOfStackSymbols; stackSymbol++)
        {
            transition.add(
                common::transition::State{state}, common::symbol::StackSymbol{stackSymbol},
                popStack, popStackState);
        }
    }

    auto init = combineStates(vpa->initialState, secondVpa.initialState);
    return VPA{
        transition, init, acceptingStates,
        static_cast<uint16_t>(combinedVpaNumOfStates + 1)}; // +1 for poping stack state
}

void Converter::insertNonTerminalIfNeeded(const cfg::NonTerminal nonTerminal)
{
    if (seenNonTerminals.insert(nonTerminal).second)
    {
        nonTerminalsQueue.push(nonTerminal);
    }
}

std::tuple<size_t, size_t> Converter::calculateEstimatedCfgSize()
{
    const uint16_t stackSymbolsNumber{
        static_cast<uint16_t>((numOfStackSymbols + 1) * (numOfStackSymbols + 1))};

    const size_t projCalls =
        static_cast<size_t>(combinedVpaNumOfStates) * static_cast<size_t>(stackSymbolsNumber) *
        static_cast<size_t>(numOfCalls) * static_cast<size_t>(combinedVpaNumOfStates) *
        static_cast<size_t>(combinedVpaNumOfStates);

    const size_t projLocals =
        static_cast<size_t>(combinedVpaNumOfStates) * static_cast<size_t>(stackSymbolsNumber) *
        static_cast<size_t>(numOfLocals) * static_cast<size_t>(combinedVpaNumOfStates);

    const size_t projReturnsBottom = static_cast<size_t>(combinedVpaNumOfStates) *
                                     static_cast<size_t>(combinedVpaNumOfStates) *
                                     static_cast<size_t>(numOfReturns + 1);

    const size_t projReturnsOther = static_cast<size_t>(combinedVpaNumOfStates) *
                                    static_cast<size_t>(stackSymbolsNumber - 1) *
                                    static_cast<size_t>(numOfReturns + 1);

    const size_t projReturns = projReturnsBottom + projReturnsOther;

    const size_t projCommon =
        static_cast<size_t>(combinedVpaNumOfStates) * static_cast<size_t>(stackSymbolsNumber) +
        static_cast<size_t>(combinedVpaNumOfStates);

    const size_t estimatedNumberOfProjections =
        (projCalls + projLocals + projReturns + projCommon) / 4 * 3;

    const size_t estimatedNonTerminals =
        (static_cast<size_t>(combinedVpaNumOfStates + 1) * static_cast<size_t>(stackSymbolsNumber) *
             static_cast<size_t>(combinedVpaNumOfStates + 1) +
         1) /
        4 * 3;

    // return {estimatedNumberOfProjections, estimatedNonTerminals};
    return {20000, 1000};
}

std::tuple<common::transition::State, common::symbol::StackSymbol, common::transition::State>
Converter::decodeNonTerminal(const cfg::NonTerminal nonTerminal)
{
    uint64_t q1 = (nonTerminal / (1ULL << 34)) - 1;
    uint64_t sId = ((nonTerminal % (1ULL << 34)) / (1ULL << 17)) - 1;
    uint64_t q2 = (nonTerminal % (1ULL << 17)) - 1;
    return {
        common::transition::State{static_cast<uint16_t>(q1)},
        common::symbol::StackSymbol{static_cast<uint16_t>(sId)},
        common::transition::State{static_cast<uint16_t>(q2)}};
}

std::shared_ptr<cfg::Cfg> Converter::convertVpaToCfg(const VPA &vpa)
{
    TIME_MARKER("convertVpaToCfg");
    LOG("[Converter]: converting VPA to CFG");

    combinedVpaNumOfStates = vpa.numOfStates;

    const auto [estimatedNumberOfProjections, estimatedNonTerminals] = calculateEstimatedCfgSize();
    cfg = std::make_shared<cfg::Cfg>(estimatedNumberOfProjections, estimatedNonTerminals);
    seenNonTerminals.clear();
    seenNonTerminals.reserve(estimatedNonTerminals);

    addCommonProjections(vpa);
    while (!nonTerminalsQueue.empty())
    {
        const auto nonTerminal{nonTerminalsQueue.front()};
        nonTerminalsQueue.pop();

        const auto [state1, stackSymbol, state2] = decodeNonTerminal(nonTerminal);

        addCallProjections(vpa.delta.callT[state1], nonTerminal, state1, stackSymbol, state2);
        addReturnProjections(
            vpa.delta.returnT[state1][stackSymbol], nonTerminal, state1, stackSymbol, state2);
        addLocalProjections(vpa.delta.localT[state1], nonTerminal, state1, stackSymbol, state2);
    }

    return cfg;
}

void Converter::addCalls(uint16_t state, const VPA &secondVpa)
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        auto states{convertCombinedStateIntoStates(state)};
        const common::symbol::CallSymbol callSymbol{call};

        common::transition::CoArgument coArg1{vpa->delta(states.first, callSymbol)};
        if (coArg1.state == common::transition::State::INVALID)
        {
            coArg1.state = vpa->getSink();
        }
        if (coArg1.stackSymbol == common::symbol::StackSymbol::INVALID)
        {
            coArg1.stackSymbol = common::symbol::StackSymbol{numOfStackSymbols};
        }

        common::transition::CoArgument coArg2{secondVpa.delta(states.second, callSymbol)};
        if (coArg2.state == common::transition::State::INVALID)
        {
            coArg2.state = secondVpa.getSink();
        }
        if (coArg2.stackSymbol == common::symbol::StackSymbol::INVALID)
        {
            coArg2.stackSymbol = common::symbol::StackSymbol{numOfStackSymbols};
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
        if (coArg1 == common::transition::State::INVALID)
        {
            coArg1 = vpa->getSink();
        }
        common::transition::State coArg2{secondVpa.delta(states.second, localSymbol)};
        if (coArg2 == common::transition::State::INVALID)
        {
            coArg2 = secondVpa.getSink();
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
        if (coArg1 == common::transition::State::INVALID)
        {
            coArg1 = vpa->getSink();
        }

        common::transition::State coArg2{
            secondVpa.delta(states.second, stackSymbols.second, returnSymbol)};
        if (coArg2 == common::transition::State::INVALID)
        {
            coArg2 = secondVpa.getSink();
        }

        transition.add(
            common::transition::State{state}, common::symbol::StackSymbol{stackSymbol},
            returnSymbol, combineStates(coArg1, coArg2));
    }
}

std::pair<common::transition::State, common::transition::State>
Converter::convertCombinedStateIntoStates(uint16_t state) const
{
    const uint16_t numOfStates{static_cast<uint16_t>(vpa->numOfStates + 1)};
    return {
        common::transition::State{static_cast<uint16_t>(state % numOfStates)},
        common::transition::State{static_cast<uint16_t>(state / numOfStates)}};
}

std::pair<common::symbol::StackSymbol, common::symbol::StackSymbol>
Converter::convertCombinedStackSymbolIntoSymbols(uint16_t stackSymbol) const
{
    if (stackSymbol == symbol::StackSymbol::BOTTOM)
    {
        return {symbol::StackSymbol::BOTTOM, symbol::StackSymbol::BOTTOM};
    }

    return {
        common::symbol::StackSymbol{static_cast<uint16_t>((stackSymbol % (numOfStackSymbols + 1)))},
        common::symbol::StackSymbol{
            static_cast<uint16_t>((stackSymbol / (numOfStackSymbols + 1)))}};
}

common::transition::State Converter::combineStates(uint16_t s1, uint16_t s2) const
{
    return common::transition::State{static_cast<uint16_t>(s2 * (vpa->numOfStates + 1) + s1)};
}

common::symbol::StackSymbol Converter::combineStackSymbols(uint16_t s1, uint16_t s2) const
{
    if (s1 == common::symbol::StackSymbol::BOTTOM)
    {
        assert(s2 == common::symbol::StackSymbol::BOTTOM);
        return common::symbol::StackSymbol::BOTTOM;
    }

    return common::symbol::StackSymbol{static_cast<uint16_t>((s2 * (numOfStackSymbols + 1)) + s1)};
}

bool Converter::isAcceptingState(uint16_t state, const VPA &secondVpa) const
{
    std::pair<common::transition::State, common::transition::State> states{
        convertCombinedStateIntoStates(state)};

    return vpa->acceptingStates.at(states.first) ^ secondVpa.acceptingStates.at(states.second);
}

void printNonTerminal(std::ostream &os, cfg::NonTerminal nt)
{
    if (nt == cfg::NonTerminal::START)
    {
        os << "{START}";
        return;
    }
    uint64_t q1 = (nt / (1ULL << 34)) - 1;
    uint64_t sId = ((nt % (1ULL << 34)) / (1ULL << 17)) - 1;
    uint64_t q2 = (nt % (1ULL << 17)) - 1;

    os << "{q1=" << q1 << ", sId=" << sId << ", q2=" << q2 << "}";
}

std::ostream &operator<<(std::ostream &os, const cfg::NonTerminal &nt)
{
    printNonTerminal(os, nt);
    return os;
}

void Converter::addCallProjections(
    const common::transition::CoArgument (&callT)[utils::MaxNumOfLetters],
    const cfg::NonTerminal nonTerminal, const common::transition::State state,
    const common::symbol::StackSymbol stackSymbol, const common::transition::State state2)
{
    for (uint16_t callId = 0; callId < numOfCalls; callId++)
    {
        cfg::Terminal terminal{
            cfg::Calculator::convertSymbolToTerminal(common::symbol::CallSymbol{callId})};

        common::transition::CoArgument coArg{callT[callId]};

        for (uint16_t stateId = 0; stateId < combinedVpaNumOfStates; stateId++)
        {
            transition::State secondState{stateId};

            cfg::NonTerminal nonTerminal1{
                cfg::Calculator::makeNonTerminal(coArg.state, coArg.stackSymbol, secondState)};
            cfg::NonTerminal nonTerminal2{
                cfg::Calculator::makeNonTerminal(secondState, stackSymbol, state2)};

            insertNonTerminalIfNeeded(nonTerminal1);
            insertNonTerminalIfNeeded(nonTerminal2);

            cfg->addProjection(nonTerminal, terminal, nonTerminal1, nonTerminal2);
        }
    }
}

void Converter::addReturnProjections(
    const common::transition::State (&returnT)[utils::MaxNumOfCombinedAutomatonLetters],
    const cfg::NonTerminal nonTerminal, const common::transition::State state,
    const common::symbol::StackSymbol stackSymbol, const common::transition::State state2)
{
    for (uint16_t returnId = 0; returnId <= numOfReturns; returnId++) // <= for poping states
    {
        common::symbol::ReturnSymbol returnSymbol{returnId};
        if (stackSymbol == symbol::StackSymbol::BOTTOM)
        {
            cfg::NonTerminal nonTerminal1{
                cfg::Calculator::makeNonTerminal(returnT[returnSymbol], stackSymbol, state2)};

            insertNonTerminalIfNeeded(nonTerminal1);

            cfg->addProjection(
                nonTerminal, cfg::Calculator::convertSymbolToTerminal(returnSymbol), nonTerminal1,
                std::nullopt);
        }
        else
        {
            if (returnT[returnSymbol] == state2)
            {
                cfg->addProjection(
                    nonTerminal, cfg::Calculator::convertSymbolToTerminal(returnSymbol),
                    std::nullopt, std::nullopt);
            }
        }
    }
}

void Converter::addLocalProjections(
    const common::transition::State (&localT)[utils::MaxNumOfLetters],
    const cfg::NonTerminal nonTerminal, const common::transition::State state,
    const common::symbol::StackSymbol stackSymbol, const common::transition::State state2)
{
    for (uint16_t localId = 0; localId < numOfLocals; localId++)
    {
        common::symbol::LocalSymbol localSymbol{localId};

        const cfg::NonTerminal nonTerminal1{
            cfg::Calculator::makeNonTerminal(localT[localSymbol], stackSymbol, state2)};

        insertNonTerminalIfNeeded(nonTerminal1);

        cfg->addProjection(
            nonTerminal, cfg::Calculator::convertSymbolToTerminal(localSymbol), nonTerminal1,
            std::nullopt);
    }
}

void Converter::addCommonProjections(const VPA &vpa)
{
    for (uint16_t stateId = 0; stateId < vpa.numOfStates; stateId++)
    {
        if (vpa.acceptingStates[stateId])
        {
            transition::State state{stateId};

            const auto nonTerminal{cfg::Calculator::makeNonTerminal(
                vpa.initialState, symbol::StackSymbol::BOTTOM, state)};

            if (seenNonTerminals.insert(nonTerminal).second)
            {
                nonTerminalsQueue.push(nonTerminal);
            }

            cfg->addProjection(cfg::NonTerminal::START, std::nullopt, nonTerminal, std::nullopt);

            for (uint16_t stackSymbol = 0; stackSymbol < combinedVpaNumOfStackSymbols;
                 stackSymbol++)
            {
                symbol::StackSymbol ss{stackSymbol};
                cfg::NonTerminal nonTerminal{cfg::Calculator::makeNonTerminal(state, ss, state)};
                cfg->addProjection(nonTerminal, std::nullopt, std::nullopt, std::nullopt);
            }
        }
    }
}
} // namespace teacher
