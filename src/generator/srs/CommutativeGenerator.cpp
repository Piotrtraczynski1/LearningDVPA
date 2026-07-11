#include "generator/srs/CommutativeGenerator.hpp"

#include <algorithm>

#include "utils/Constants.hpp"
#include "utils/ExitCode.hpp"
#include "utils/log.hpp"

namespace generator::srs
{
CommutativeGenerator::ComponentTransitions::ComponentTransitions(
    const uint16_t numOfStates_, const uint16_t callBegin_, const uint16_t callEnd_,
    const uint16_t localBegin_, const uint16_t localEnd_, const uint16_t returnBegin_,
    const uint16_t returnEnd_, const uint16_t stackBegin_, const uint16_t stackEnd_,
    const uint16_t numOfCalls, const uint16_t numOfLocals, const uint16_t numOfReturns,
    const uint16_t numOfStackSymbols)
    : numOfStates{numOfStates_}, callBegin{callBegin_}, callEnd{callEnd_}, localBegin{localBegin_},
      localEnd{localEnd_}, returnBegin{returnBegin_}, returnEnd{returnEnd_},
      stackBegin{stackBegin_}, stackEnd{stackEnd_},
      calls(static_cast<size_t>(numOfStates) * numOfCalls),
      locals(static_cast<size_t>(numOfStates) * numOfLocals, State::INVALID),
      returns(static_cast<size_t>(numOfStates) * numOfStackSymbols * numOfReturns, State::INVALID)
{
}

void CommutativeGenerator::validateGeneratorConfig(
    uint16_t &numOfStates_, [[maybe_unused]] uint16_t &numOfCalls_,
    [[maybe_unused]] uint16_t &numOfLocals_, [[maybe_unused]] uint16_t &numOfReturns_,
    uint16_t &numOfStackSymbols_)
{
    constexpr uint16_t minNumOfStackSymbols{3};
    if (numOfStackSymbols < minNumOfStackSymbols)
    {
        IMP("[CommutativeGenerator] adjusting stack symbols from %u to %u", numOfStackSymbols,
            minNumOfStackSymbols);
        numOfStackSymbols = minNumOfStackSymbols;
    }

    const uint16_t adjustedNumOfStates{static_cast<uint16_t>(numOfStates * secondDvpaNumOfStates)};
    if (adjustedNumOfStates > utils::MaxNumOfAutomatonStates)
    {
        ERR("[CommutativeGenerator]: numOfStates * secondDvpaNumOfStates (%u) is greater than "
            "MaxNumOfAutomatonStates! (%u)",
            adjustedNumOfStates, utils::MaxNumOfAutomatonStates);
        exit(toExit(ExitCode::GENERATOR));
    }
    if (numOfStackSymbols > utils::MaxNumOfStackSymbols)
    {
        ERR("[CommutativeGenerator]: numOfStackSymbols (%u) is greater than "
            "MaxNumOfStackSymbols! (%u)",
            numOfStackSymbols, utils::MaxNumOfStackSymbols);
        exit(toExit(ExitCode::GENERATOR));
    }

    IMP("[CommutativeGenerator] adjusting numOfStates to %u", adjustedNumOfStates);
    numOfStates_ = adjustedNumOfStates;
    numOfStackSymbols_ = numOfStackSymbols;
}

bool CommutativeGenerator::generatorSpecificCheck(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    return numOfStates * secondDvpaNumOfStates + 1 >= hypothesis->getNumOfStates();
}

void CommutativeGenerator::splitAlphabet()
{
    callSplitPoint = numOfCalls / 2;
    returnSplitPoint = numOfReturns / 2;
    localSplitPoint = numOfLocals / 2;
    stackSplitPoint = static_cast<uint16_t>(1 + (numOfStackSymbols - 1) / 2);
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> CommutativeGenerator::run()
{
    splitAlphabet();

    auto first = generateComponent(
        numOfStates, 0, callSplitPoint, 0, localSplitPoint, 0, returnSplitPoint, 1,
        stackSplitPoint);
    auto second = generateComponent(
        secondDvpaNumOfStates, callSplitPoint, numOfCalls, localSplitPoint, numOfLocals,
        returnSplitPoint, numOfReturns, stackSplitPoint, numOfStackSymbols);

    generateProduct(first, second);
    selectProductAcceptingStates(first.numOfStates, second.numOfStates);

    const uint16_t productNumOfStates{
        static_cast<uint16_t>(first.numOfStates * second.numOfStates)};
    return std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, acceptingStates, productNumOfStates);
}

CommutativeGenerator::ComponentTransitions CommutativeGenerator::generateComponent(
    const uint16_t componentNumOfStates, const uint16_t callBegin, const uint16_t callEnd,
    const uint16_t localBegin, const uint16_t localEnd, const uint16_t returnBegin,
    const uint16_t returnEnd, const uint16_t stackBegin, const uint16_t stackEnd)
{
    ComponentTransitions component{
        componentNumOfStates, callBegin,    callEnd,          localBegin, localEnd,
        returnBegin,          returnEnd,    stackBegin,       stackEnd,   numOfCalls,
        numOfLocals,          numOfReturns, numOfStackSymbols};
    generateComponentTransitions(component);
    return component;
}

void CommutativeGenerator::generateComponentTransitions(ComponentTransitions &component)
{
    for (uint16_t stateId = 0; stateId < component.numOfStates; stateId++)
    {
        const State state{stateId};
        addCalls(component, state);
        addLocals(component, state);
        addReturns(component, state);
    }
}

void CommutativeGenerator::addCalls(ComponentTransitions &component, const State state)
{
    for (uint16_t call = component.callBegin; call < component.callEnd; call++)
    {
        if (skipTransition())
        {
            continue;
        }

        component.calls[callIndex(state, call)] = {
            State{static_cast<uint16_t>(rng() % component.numOfStates)},
            StackSymbol{static_cast<uint16_t>(
                component.stackBegin + rng() % (component.stackEnd - component.stackBegin))}};
    }
}

void CommutativeGenerator::addLocals(ComponentTransitions &component, const State state)
{
    for (uint16_t local = component.localBegin; local < component.localEnd; local++)
    {
        if (skipTransition())
        {
            continue;
        }

        component.locals[localIndex(state, local)] =
            State{static_cast<uint16_t>(rng() % component.numOfStates)};
    }
}

void CommutativeGenerator::addReturns(ComponentTransitions &component, const State state)
{
    for (uint16_t ret = component.returnBegin; ret < component.returnEnd; ret++)
    {
        for (uint16_t stackSymbol = component.stackBegin; stackSymbol < component.stackEnd;
             stackSymbol++)
        {
            if (skipTransition())
            {
                continue;
            }

            component.returns[returnIndex(state, stackSymbol, ret)] =
                State{static_cast<uint16_t>(rng() % component.numOfStates)};
        }
    }
}

void CommutativeGenerator::generateProduct(
    const ComponentTransitions &first, const ComponentTransitions &second)
{
    transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

    for (uint16_t secondState = 0; secondState < second.numOfStates; secondState++)
    {
        for (uint16_t firstState = 0; firstState < first.numOfStates; firstState++)
        {
            const State source{combineStates(firstState, secondState)};
            addProductCalls(source, firstState, secondState, first, second);
            addProductLocals(source, firstState, secondState, first, second);
            addProductReturns(source, firstState, secondState, first, second);
        }
    }
}

void CommutativeGenerator::addProductCalls(
    const State source, const uint16_t firstState, const uint16_t secondState,
    const ComponentTransitions &first, const ComponentTransitions &second)
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        const bool belongsToFirst{call < callSplitPoint};
        const auto &componentTransition = belongsToFirst
                                              ? first.calls[callIndex(firstState, call)]
                                              : second.calls[callIndex(secondState, call)];
        if (componentTransition.destination == State::INVALID)
        {
            continue;
        }

        const State destination = belongsToFirst
                                      ? combineStates(componentTransition.destination, secondState)
                                      : combineStates(firstState, componentTransition.destination);
        transition->add(
            source, common::symbol::CallSymbol{call}, destination, componentTransition.stackSymbol);
    }
}

void CommutativeGenerator::addProductLocals(
    const State source, const uint16_t firstState, const uint16_t secondState,
    const ComponentTransitions &first, const ComponentTransitions &second)
{
    for (uint16_t local = 0; local < numOfLocals; local++)
    {
        const bool belongsToFirst{local < localSplitPoint};
        const State componentDestination = belongsToFirst
                                               ? first.locals[localIndex(firstState, local)]
                                               : second.locals[localIndex(secondState, local)];
        if (componentDestination == State::INVALID)
        {
            continue;
        }

        const State destination = belongsToFirst ? combineStates(componentDestination, secondState)
                                                 : combineStates(firstState, componentDestination);
        transition->add(source, common::symbol::LocalSymbol{local}, destination);
    }
}

void CommutativeGenerator::addProductReturns(
    const State source, const uint16_t firstState, const uint16_t secondState,
    const ComponentTransitions &first, const ComponentTransitions &second)
{
    for (uint16_t ret = 0; ret < numOfReturns; ret++)
    {
        const bool belongsToFirst{ret < returnSplitPoint};
        const auto &component{belongsToFirst ? first : second};
        const uint16_t componentState{belongsToFirst ? firstState : secondState};

        for (uint16_t stackSymbol = component.stackBegin; stackSymbol < component.stackEnd;
             stackSymbol++)
        {
            const State componentDestination =
                component.returns[returnIndex(componentState, stackSymbol, ret)];
            if (componentDestination == State::INVALID)
            {
                continue;
            }

            const State destination = belongsToFirst
                                          ? combineStates(componentDestination, secondState)
                                          : combineStates(firstState, componentDestination);
            transition->add(
                source, StackSymbol{stackSymbol}, common::symbol::ReturnSymbol{ret}, destination);
        }
    }
}

void CommutativeGenerator::selectProductAcceptingStates(
    const uint16_t firstNumOfStates, const uint16_t secondNumOfStates)
{
    const auto firstAccepting{selectComponentAcceptingStates(firstNumOfStates)};
    const auto secondAccepting{selectComponentAcceptingStates(secondNumOfStates)};

    acceptingStates.clear();
    for (uint16_t secondState = 0; secondState < secondNumOfStates; secondState++)
    {
        for (uint16_t firstState = 0; firstState < firstNumOfStates; firstState++)
        {
            if (firstAccepting[firstState] or secondAccepting[secondState])
            {
                acceptingStates.push_back(combineStates(firstState, secondState));
            }
        }
    }
}

std::vector<bool> CommutativeGenerator::selectComponentAcceptingStates(
    const uint16_t componentNumOfStates)
{
    std::vector<bool> componentAccepting(componentNumOfStates);
    for (auto &&accepting : componentAccepting)
    {
        accepting = shouldAccept();
    }

    if (std::all_of(
            componentAccepting.begin(), componentAccepting.end(), [](bool value) { return value; }))
    {
        componentAccepting.back() = false;
    }
    if (std::none_of(
            componentAccepting.begin(), componentAccepting.end(), [](bool value) { return value; }))
    {
        componentAccepting.front() = true;
    }

    return componentAccepting;
}

size_t CommutativeGenerator::callIndex(const uint16_t state, const uint16_t call) const
{
    return static_cast<size_t>(state) * numOfCalls + call;
}

size_t CommutativeGenerator::localIndex(const uint16_t state, const uint16_t local) const
{
    return static_cast<size_t>(state) * numOfLocals + local;
}

size_t CommutativeGenerator::returnIndex(
    const uint16_t state, const uint16_t stackSymbol, const uint16_t ret) const
{
    return (static_cast<size_t>(state) * numOfStackSymbols + stackSymbol) * numOfReturns + ret;
}

CommutativeGenerator::State CommutativeGenerator::combineStates(
    const uint16_t first, const uint16_t second) const
{
    return State{static_cast<uint16_t>(second * numOfStates + first)};
}

learner::srs::Srs CommutativeGenerator::getSrs()
{
    learner::srs::Srs srs{};

    for (uint16_t firstLocal = 0; firstLocal < localSplitPoint; firstLocal++)
    {
        for (uint16_t secondLocal = localSplitPoint; secondLocal < numOfLocals; secondLocal++)
        {
            srs.push_back(
                learner::srs::SrsRuleWithParams{
                    .left =
                        {.left = {common::Word{common::symbol::LocalSymbol{firstLocal}}},
                         .right = {common::Word{common::symbol::LocalSymbol{secondLocal}}},
                         .takesParams = false},
                    .right = {
                        .left = {common::Word{common::symbol::LocalSymbol{secondLocal}}},
                        .right = {common::Word{common::symbol::LocalSymbol{firstLocal}}},
                        .takesParams = false}});
        }
    }

    for (uint16_t firstCall = 0; firstCall < callSplitPoint; firstCall++)
    {
        for (uint16_t firstReturn = 0; firstReturn < returnSplitPoint; firstReturn++)
        {
            for (uint16_t secondCall = callSplitPoint; secondCall < numOfCalls; secondCall++)
            {
                for (uint16_t secondReturn = returnSplitPoint; secondReturn < numOfReturns;
                     secondReturn++)
                {
                    srs.push_back(
                        learner::srs::SrsRuleWithParams{
                            .left =
                                {.left =
                                     {common::Word{common::symbol::CallSymbol{firstCall}} +
                                      common::Word{common::symbol::CallSymbol{secondCall}}},
                                 .right = common::Word{common::symbol::ReturnSymbol{secondReturn}} +
                                          common::Word{common::symbol::ReturnSymbol{firstReturn}}},
                            .right = {
                                .left =
                                    {common::Word{common::symbol::CallSymbol{secondCall}} +
                                     common::Word{common::symbol::CallSymbol{firstCall}}},
                                .right =
                                    common::Word{common::symbol::ReturnSymbol{firstReturn}} +
                                    common::Word{common::symbol::ReturnSymbol{secondReturn}}}});
                }
            }
        }
    }

    return srs;
}
} // namespace generator::srs
