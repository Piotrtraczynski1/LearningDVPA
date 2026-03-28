#include "teacher/Converter.hpp"

#include "teacher/cfg/Calculator.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace teacher
{
VPA<AutomatonKind::Combined> Converter::combineVPA(const VPA<AutomatonKind::Normal> &secondVpa)
{
    TIME_MARKER("[Converter]: combineVPA");
    transition = std::make_unique<common::transition::Transition<AutomatonKind::Combined>>();

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
            transition->add(
                common::transition::State{state}, common::symbol::StackSymbol{stackSymbol},
                popStack, popStackState);
        }
    }

    auto init = combineStates(vpa->initialState, secondVpa.initialState);
    return VPA<AutomatonKind::Combined>{
        std::move(transition), init, acceptingStates,
        static_cast<uint16_t>(combinedVpaNumOfStates + 1)}; // +1 for poping stack state
}

void Converter::insertNonTerminalIfNeeded(const cfg::NonTerminal nonTerminal)
{
    if (not seenNonTerminals[static_cast<uint32_t>(nonTerminal)])
    {
        seenNonTerminals[static_cast<uint32_t>(nonTerminal)] = 1;
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

    const size_t estimatedNumberOfProductions =
        (projCalls + projLocals + projReturns + projCommon) / 4 * 3;

    const size_t estimatedNonTerminals =
        (static_cast<size_t>(combinedVpaNumOfStates + 1) * static_cast<size_t>(stackSymbolsNumber) *
             static_cast<size_t>(combinedVpaNumOfStates + 1) +
         1) /
        4 * 3;

    return {estimatedNumberOfProductions, estimatedNonTerminals};
}

std::shared_ptr<cfg::Cfg> Converter::convertVpaToCfg(const VPA<AutomatonKind::Combined> &vpa)
{
    TIME_MARKER("[Converter]: convertVpaToCfg");
    LOG("[Converter]: converting VPA to CFG");

    combinedVpaNumOfStates = vpa.numOfStates;

    const auto [estimatedNumberOfProductions, estimatedNonTerminals] = calculateEstimatedCfgSize();
    cfg = std::make_shared<cfg::Cfg>(estimatedNumberOfProductions, estimatedNonTerminals);
    seenNonTerminals.reset();

    addCommonProductions(vpa);
    while (not nonTerminalsQueue.empty())
    {
        const auto nonTerminal{nonTerminalsQueue.front()};
        nonTerminalsQueue.pop();

        const auto [state1, stackSymbol, state2] = cfg::Calculator::decodeNonTerminal(nonTerminal);

        addCallProductions((*vpa.delta).callT[state1], nonTerminal, stackSymbol, state2);
        addReturnProductions(
            (*vpa.delta).returnT[state1][stackSymbol], nonTerminal, stackSymbol, state2);
        addLocalProductions((*vpa.delta).localT[state1], nonTerminal, stackSymbol, state2);
    }

    return cfg;
}

void Converter::addCalls(uint16_t state, const VPA<AutomatonKind::Normal> &secondVpa)
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        auto states{convertCombinedStateIntoStates(state)};
        const common::symbol::CallSymbol callSymbol{call};

        common::transition::CoArgument coArg1{(*vpa->delta)(states.first, callSymbol)};
        if (coArg1.state == common::transition::State::INVALID)
        {
            coArg1.state = vpa->getSink();
        }
        if (coArg1.stackSymbol == common::symbol::StackSymbol::INVALID)
        {
            coArg1.stackSymbol = common::symbol::StackSymbol{numOfStackSymbols};
        }

        common::transition::CoArgument coArg2{(*secondVpa.delta)(states.second, callSymbol)};
        if (coArg2.state == common::transition::State::INVALID)
        {
            coArg2.state = secondVpa.getSink();
        }
        if (coArg2.stackSymbol == common::symbol::StackSymbol::INVALID)
        {
            coArg2.stackSymbol = common::symbol::StackSymbol{numOfStackSymbols};
        }

        transition->add(
            common::transition::State{state}, callSymbol, combineStates(coArg1.state, coArg2.state),
            combineStackSymbols(coArg1.stackSymbol, coArg2.stackSymbol));
    }
}

void Converter::addLocals(uint16_t state, const VPA<AutomatonKind::Normal> &secondVpa)
{
    for (uint16_t local = 0; local < numOfLocals; local++)
    {
        auto states{convertCombinedStateIntoStates(state)};
        const common::symbol::LocalSymbol localSymbol{local};

        common::transition::State coArg1{(*vpa->delta)(states.first, localSymbol)};
        if (coArg1 == common::transition::State::INVALID)
        {
            coArg1 = vpa->getSink();
        }
        common::transition::State coArg2{(*secondVpa.delta)(states.second, localSymbol)};
        if (coArg2 == common::transition::State::INVALID)
        {
            coArg2 = secondVpa.getSink();
        }

        transition->add(
            common::transition::State{state}, localSymbol, combineStates(coArg1, coArg2));
    }
}

void Converter::addReturns(
    uint16_t state, uint16_t stackSymbol, const VPA<AutomatonKind::Normal> &secondVpa)
{
    for (uint16_t ret = 0; ret < numOfReturns; ret++)
    {
        auto stackSymbols{convertCombinedStackSymbolIntoSymbols(stackSymbol)};
        auto states{convertCombinedStateIntoStates(state)};

        const common::symbol::ReturnSymbol returnSymbol{ret};

        common::transition::State coArg1{
            (*vpa->delta)(states.first, stackSymbols.first, returnSymbol)};
        if (coArg1 == common::transition::State::INVALID)
        {
            coArg1 = vpa->getSink();
        }

        common::transition::State coArg2{
            (*secondVpa.delta)(states.second, stackSymbols.second, returnSymbol)};
        if (coArg2 == common::transition::State::INVALID)
        {
            coArg2 = secondVpa.getSink();
        }

        transition->add(
            common::transition::State{state}, common::symbol::StackSymbol{stackSymbol},
            returnSymbol, combineStates(coArg1, coArg2));
    }
}

std::pair<common::transition::State, common::transition::State> Converter::
    convertCombinedStateIntoStates(uint16_t state) const
{
    const uint16_t numOfStates{static_cast<uint16_t>(vpa->numOfStates + 1)};
    return {
        common::transition::State{static_cast<uint16_t>(state % numOfStates)},
        common::transition::State{static_cast<uint16_t>(state / numOfStates)}};
}

std::pair<common::symbol::StackSymbol, common::symbol::StackSymbol> Converter::
    convertCombinedStackSymbolIntoSymbols(uint16_t stackSymbol) const
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
        return common::symbol::StackSymbol::BOTTOM;
    }

    return common::symbol::StackSymbol{static_cast<uint16_t>((s2 * (numOfStackSymbols + 1)) + s1)};
}

bool Converter::isAcceptingState(uint16_t state, const VPA<AutomatonKind::Normal> &secondVpa) const
{
    std::pair<common::transition::State, common::transition::State> states{
        convertCombinedStateIntoStates(state)};

    return vpa->acceptingStates.at(states.first) ^ secondVpa.acceptingStates.at(states.second);
}

void Converter::addCallProductions(
    const common::transition::CoArgument (&callT)[utils::MaxNumOfCombinedAutomatonLetters],
    const cfg::NonTerminal nonTerminal, const common::symbol::StackSymbol stackSymbol,
    const common::transition::State state)
{
    for (uint16_t callId = 0; callId < numOfCalls; callId++)
    {
        const common::transition::CoArgument &coArg{callT[callId]};

        for (uint16_t stateId = 0; stateId < combinedVpaNumOfStates; stateId++)
        {
            transition::State secondState{stateId};

            cfg::NonTerminal nonTerminal1{
                cfg::Calculator::makeNonTerminal(coArg.state, coArg.stackSymbol, secondState)};
            cfg::NonTerminal nonTerminal2{
                cfg::Calculator::makeNonTerminal(secondState, stackSymbol, state)};

            insertNonTerminalIfNeeded(nonTerminal1);
            insertNonTerminalIfNeeded(nonTerminal2);

            cfg->addProduction(
                nonTerminal,
                cfg::Calculator::convertSymbolToTerminal(common::symbol::CallSymbol{callId}),
                nonTerminal1, nonTerminal2);
        }
    }
}

void Converter::addReturnProductions(
    const common::transition::State (&returnT)[utils::MaxNumOfCombinedAutomatonLetters],
    const cfg::NonTerminal nonTerminal, const common::symbol::StackSymbol stackSymbol,
    const common::transition::State state)
{
    for (uint16_t returnId = 0; returnId <= numOfReturns; returnId++) // <= for poping states
    {
        common::symbol::ReturnSymbol returnSymbol{returnId};
        if (stackSymbol == symbol::StackSymbol::BOTTOM)
        {
            cfg::NonTerminal nonTerminal1{
                cfg::Calculator::makeNonTerminal(returnT[returnSymbol], stackSymbol, state)};

            insertNonTerminalIfNeeded(nonTerminal1);

            cfg->addProduction(
                nonTerminal, cfg::Calculator::convertSymbolToTerminal(returnSymbol), nonTerminal1,
                cfg::NonTerminal::INVALID);
        }
        else
        {
            if (returnT[returnSymbol] == state)
            {
                cfg->addProduction(
                    nonTerminal, cfg::Calculator::convertSymbolToTerminal(returnSymbol),
                    cfg::NonTerminal::INVALID, cfg::NonTerminal::INVALID);
            }
        }
    }
}

void Converter::addLocalProductions(
    const common::transition::State (&localT)[utils::MaxNumOfCombinedAutomatonLetters],
    const cfg::NonTerminal nonTerminal, const common::symbol::StackSymbol stackSymbol,
    const common::transition::State state)
{
    for (uint16_t localId = 0; localId < numOfLocals; localId++)
    {
        common::symbol::LocalSymbol localSymbol{localId};

        const cfg::NonTerminal nonTerminal1{
            cfg::Calculator::makeNonTerminal(localT[localSymbol], stackSymbol, state)};

        insertNonTerminalIfNeeded(nonTerminal1);

        cfg->addProduction(
            nonTerminal, cfg::Calculator::convertSymbolToTerminal(localSymbol), nonTerminal1,
            cfg::NonTerminal::INVALID);
    }
}

void Converter::addCommonProductions(const VPA<AutomatonKind::Combined> &vpa)
{
    for (uint16_t stateId = 0; stateId < vpa.numOfStates; stateId++)
    {
        if (vpa.acceptingStates[stateId])
        {
            transition::State state{stateId};

            const auto nonTerminal{cfg::Calculator::makeNonTerminal(
                vpa.initialState, symbol::StackSymbol::BOTTOM, state)};

            insertNonTerminalIfNeeded(nonTerminal);

            cfg->addProduction(
                cfg::NonTerminal::START, cfg::Terminal::INVALID, nonTerminal,
                cfg::NonTerminal::INVALID);

            for (uint16_t stackSymbol = 0; stackSymbol < combinedVpaNumOfStackSymbols;
                 stackSymbol++)
            {
                symbol::StackSymbol ss{stackSymbol};
                cfg::NonTerminal nonTerminal{cfg::Calculator::makeNonTerminal(state, ss, state)};
                cfg->addProduction(
                    nonTerminal, cfg::Terminal::INVALID, cfg::NonTerminal::INVALID,
                    cfg::NonTerminal::INVALID);
            }
        }
    }
}
} // namespace teacher
