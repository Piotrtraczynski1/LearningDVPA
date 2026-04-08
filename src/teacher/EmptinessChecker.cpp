#include <queue>

#include "teacher/EmptinessChecker.hpp"

namespace teacher
{
EmptinessChecker::EmptinessChecker(
    uint16_t numCallsArg, uint16_t numReturnsArg, uint16_t numLocalsArg)
    : numOfCalls{numCallsArg}, numOfReturns{numReturnsArg}, numOfLocals{numLocalsArg}
{
}

common::Word EmptinessChecker::check(
    const std::shared_ptr<common::VPA<AutomatonKind::Combined>> &vpa,
    const uint16_t stackSymbolsNumber)
{
    init(vpa, stackSymbolsNumber);

    addInitialGenerativeNonTerminals(vpa);

    while (not generativeNonTerminals.empty())
    {
        NonTerminal generative{generativeNonTerminals.front()};
        generativeNonTerminals.pop();

        const auto [leftState, _, rightState] = decodeNonTerminal(generative);
        generativeByLeftState[leftState].push_back(generative);
        generativeByRightState[rightState].push_back(generative);

        addGenerativeLocals(generative);
        addGenerativeReturns(generative);
        addGenerativeCalls(generative);
    }

    for (uint16_t state = 0; state < numOfStates; state++)
    {
        if (vpa->acceptingStates[state])
        {
            const auto start{
                encodeNonTerminal(vpa->initialState, common::symbol::StackSymbol::BOTTOM, state)};
            if (generative[start])
            {
                common::Word example{};
                buildExample(start, example);
                return example;
            }
        }
    }

    return common::Word{};
}

void EmptinessChecker::init(
    const std::shared_ptr<common::VPA<AutomatonKind::Combined>> &vpa,
    const uint16_t stackSymbolsNumber)
{
    numOfStackSymbols = stackSymbolsNumber;
    numOfStates = vpa->getNumOfStates();
    generativeNonTerminals = std::queue<NonTerminal>{};

    uint64_t maxNumOfNonTerminals{
        static_cast<uint64_t>(numOfStates) * numOfStackSymbols * numOfStates};
    generative.assign(maxNumOfNonTerminals, false);
    witnesses.assign(maxNumOfNonTerminals, Witness{});

    localParents.assign(numOfStates, {});
    returnParents.assign(numOfStates, {});
    callParents.assign(static_cast<size_t>(numOfStates) * numOfStackSymbols, {});

    generativeByLeftState.assign(numOfStates, {});
    generativeByRightState.assign(numOfStates, {});

    initParents(*vpa);
}

void EmptinessChecker::initParents(const common::VPA<AutomatonKind::Combined> &vpa)
{
    for (uint16_t leftState = 0; leftState < numOfStates; leftState++)
    {
        for (uint16_t localSymbol = 0; localSymbol < numOfLocals; localSymbol++)
        {
            const auto rightState{vpa.delta->localT[leftState][localSymbol]};
            if (rightState != common::transition::State::INVALID)
            {
                localParents[rightState].push_back(
                    ParentStateAndSymbol{.state = leftState, .symbol = localSymbol});
            }
        }

        for (uint16_t returnSymbol = 0; returnSymbol <= numOfReturns;
             returnSymbol++) // <= for poping states
        {
            const auto rightState{
                vpa.delta->returnT[leftState][common::symbol::StackSymbol::BOTTOM][returnSymbol]};
            if (rightState != common::transition::State::INVALID)
            {
                returnParents[rightState].push_back(
                    ParentStateAndSymbol{.state = leftState, .symbol = returnSymbol});
            }
        }

        for (uint16_t callSymbol = 0; callSymbol < numOfCalls; callSymbol++)
        {
            const auto &coArg{vpa.delta->callT[leftState][callSymbol]};
            if (coArg.state != common::transition::State::INVALID and
                coArg.stackSymbol != common::symbol::StackSymbol::INVALID)
            {
                // TODO add encoding funciton
                callParents[coArg.state * numOfStackSymbols + coArg.stackSymbol].push_back(
                    ParentStateAndSymbol{.state = leftState, .symbol = callSymbol});
            }
        }
    }
}

void EmptinessChecker::addInitialGenerativeNonTerminals(
    const std::shared_ptr<common::VPA<AutomatonKind::Combined>> &vpa)
{
    for (uint16_t state = 0; state < numOfStates; state++)
    {
        if (vpa->acceptingStates[state])
        {
            addNonTerminalIfNeeded(
                state, common::symbol::StackSymbol::BOTTOM, state,
                Witness{.kind = WitnessKind::Empty});
        }

        for (uint16_t returnSymbol = 0; returnSymbol <= numOfReturns; returnSymbol++)
        {
            for (uint16_t stackSymbol = 1; stackSymbol < numOfStackSymbols;
                 stackSymbol++) // omit BOTTOM
            {
                const auto rightState{vpa->delta->returnT[state][stackSymbol][returnSymbol]};
                if (rightState != common::transition::State::INVALID)
                {
                    addNonTerminalIfNeeded(
                        state, stackSymbol, rightState,
                        Witness{.kind = WitnessKind::Return, .symbol = returnSymbol});
                }
            }
        }
    }
}

void EmptinessChecker::addNonTerminalIfNeeded(
    const uint16_t leftState, const uint16_t stackSymbol, const uint16_t rightState,
    const Witness witness)
{
    const auto nonTerminal{encodeNonTerminal(leftState, stackSymbol, rightState)};

    if (not generative[nonTerminal])
    {
        generative[nonTerminal] = true;
        generativeNonTerminals.push(nonTerminal);
        witnesses[nonTerminal] = witness;
    }
}

void EmptinessChecker::addGenerativeLocals(const NonTerminal generative)
{
    const auto [leftState, stackSymbol, rightState] = decodeNonTerminal(generative);
    for (const auto &parent : localParents[leftState])
    {
        addNonTerminalIfNeeded(
            parent.state, stackSymbol, rightState,
            Witness{.kind = WitnessKind::Local, .symbol = parent.symbol, .firstChild = generative});
    }
}

void EmptinessChecker::addGenerativeReturns(const NonTerminal generative)
{
    const auto [leftState, stackSymbol, rightState] = decodeNonTerminal(generative);
    if (stackSymbol != common::symbol::StackSymbol::BOTTOM)
    {
        return;
    }

    for (const auto parent : returnParents[leftState])
    {
        addNonTerminalIfNeeded(
            parent.state, stackSymbol, rightState,
            Witness{
                .kind = WitnessKind::ReturnBottom,
                .symbol = parent.symbol,
                .firstChild = generative});
    }
}

void EmptinessChecker::addGenerativeCalls(const NonTerminal generative)
{
    const auto [leftState, stackSymbol, rightState] = decodeNonTerminal(generative);

    // assume generative is left child
    const auto &parents{callParents[leftState * numOfStackSymbols + stackSymbol]};
    for (const auto rightChild : generativeByLeftState[rightState])
    {
        const auto [_, stackSymbol, rightState] = decodeNonTerminal(rightChild);
        for (const auto &parent : parents)
        {
            addNonTerminalIfNeeded(
                parent.state, stackSymbol, rightState,
                Witness{
                    .kind = WitnessKind::Call,
                    .symbol = parent.symbol,
                    .firstChild = generative,
                    .secondChild = rightChild});
        }
    }

    // assaume generative is right child
    for (const auto leftChild : generativeByRightState[leftState])
    {
        const auto [leftChildLeftState, leftChildStackSymbol, _] = decodeNonTerminal(leftChild);
        for (const auto &parent :
             callParents[leftChildLeftState * numOfStackSymbols + leftChildStackSymbol])
        {
            addNonTerminalIfNeeded(
                parent.state, stackSymbol, rightState,
                Witness{
                    .kind = WitnessKind::Call,
                    .symbol = parent.symbol,
                    .firstChild = leftChild,
                    .secondChild = generative});
        }
    }
}

void EmptinessChecker::buildExample(const NonTerminal nonTerminal, common::Word &example)
{
    const auto &witness{witnesses[nonTerminal]};

    // const auto [leftState, stackSymbol, rightState] = decodeNonTerminal(nonTerminal);
    // const auto [c1leftState, c1stackSymbol, c1rightState] =
    // decodeNonTerminal(witness.firstChild); const auto [c2leftState, c2stackSymbol, c2rightState]
    // = decodeNonTerminal(witness.secondChild);

    // std::cout << "buildExample: witnessKind: " << (uint16_t)witness.kind
    //           << ", symbol: " << witness.symbol << ", [" << leftState << ", " << stackSymbol <<
    //           ", "
    //           << rightState << "] leftChild: [" << c1leftState << ", " << c1stackSymbol << ", "
    //           << c1rightState << "], rightChild: [" << c2leftState << ", " << c2stackSymbol << ",
    //           "
    //           << c2rightState << "]" << std::endl;

    switch (witness.kind)
    {
    case WitnessKind::Call:
        example += common::symbol::CallSymbol{witness.symbol};
        buildExample(witness.firstChild, example);
        buildExample(witness.secondChild, example);
        return;
    case WitnessKind::Return:
        example += common::symbol::ReturnSymbol{witness.symbol};
        return;
    case WitnessKind::ReturnBottom:
        example += common::symbol::ReturnSymbol{witness.symbol};
        buildExample(witness.firstChild, example);
        return;
    case WitnessKind::Local:
        example += common::symbol::LocalSymbol{witness.symbol};
        buildExample(witness.firstChild, example);
        return;
    default:
        return;
    }
}

NonTerminal EmptinessChecker::encodeNonTerminal(
    const uint16_t leftState, const uint16_t stackSymbol, const uint16_t rightState)
{
    return static_cast<NonTerminal>(
        (static_cast<NonTerminal>(leftState) * numOfStackSymbols + stackSymbol) * numOfStates +
        rightState);
}

std::tuple<uint16_t, uint16_t, uint16_t> EmptinessChecker::decodeNonTerminal(
    const NonTerminal nonTermial)
{
    const NonTerminal tmp{nonTermial / numOfStates};

    return {
        common::transition::State{static_cast<uint16_t>(tmp / numOfStackSymbols)},
        common::symbol::StackSymbol{static_cast<uint16_t>(tmp % numOfStackSymbols)},
        common::transition::State{static_cast<uint16_t>(nonTermial % numOfStates)}};
}
} // namespace teacher