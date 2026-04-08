#include "teacher/AutomataCombiner.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace teacher
{
template <AutomatonKind Kind>
AutomataCombiner<Kind>::AutomataCombiner(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &vpa, uint16_t numCalls,
    uint16_t numReturns, uint16_t numLocals, uint16_t stackSymbolsNumber)
    : vpa{vpa}, numOfCalls{numCalls}, numOfReturns{numReturns}, numOfLocals{numLocals},
      numOfStackSymbols{stackSymbolsNumber} {};

template <AutomatonKind Kind>
std::shared_ptr<common::VPA<Kind>> AutomataCombiner<Kind>::combineVPA(
    const common::VPA<AutomatonKind::Normal> &hypothesis)
{
    TIME_MARKER("[AutomataCombiner]: combineVPA");
    transition = std::make_unique<common::transition::Transition<Kind>>();

    combinedVpaNumOfStates =
        static_cast<uint16_t>((vpa->numOfStates + 1) * (hypothesis.numOfStates + 1)); // +1 for sink
    combinedVpaNumOfStackSymbols = (numOfStackSymbols + 1) * numOfStackSymbols;

    LOG("[AutomataCombiner]: combineVpa: numOfStates: %u, stackSymbolsNumber: %u",
        combinedVpaNumOfStates + 1, combinedVpaNumOfStackSymbols);

    std::vector<uint16_t> acceptingStates{};

    for (uint16_t state = 0; state < combinedVpaNumOfStates; state++)
    {
        addCalls(state, hypothesis);
        addLocals(state, hypothesis);
        for (uint16_t stackSymbol = 0; stackSymbol < combinedVpaNumOfStackSymbols; stackSymbol++)
        {
            addReturns(state, stackSymbol, hypothesis);
        }

        if (isAcceptingState(state, hypothesis))
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

    auto init = combineStates(vpa->initialState, hypothesis.initialState);

    return std::make_shared<common::VPA<Kind>>(
        std::move(transition), init, acceptingStates,
        static_cast<uint16_t>(combinedVpaNumOfStates + 1)); // +1 for poping stack state
}

template <AutomatonKind Kind>
void AutomataCombiner<Kind>::addCalls(
    uint16_t state, const common::VPA<AutomatonKind::Normal> &secondVpa)
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

template <AutomatonKind Kind>
void AutomataCombiner<Kind>::addLocals(
    uint16_t state, const common::VPA<AutomatonKind::Normal> &secondVpa)
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

template <AutomatonKind Kind>
void AutomataCombiner<Kind>::addReturns(
    uint16_t state, uint16_t stackSymbol, const common::VPA<AutomatonKind::Normal> &secondVpa)
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

template <AutomatonKind Kind>
bool AutomataCombiner<Kind>::isAcceptingState(
    uint16_t state, const common::VPA<AutomatonKind::Normal> &secondVpa) const
{
    std::pair<common::transition::State, common::transition::State> states{
        convertCombinedStateIntoStates(state)};

    return vpa->acceptingStates.at(states.first) ^ secondVpa.acceptingStates.at(states.second);
}

template <AutomatonKind Kind>
std::pair<common::transition::State, common::transition::State> AutomataCombiner<
    Kind>::convertCombinedStateIntoStates(uint16_t state) const
{
    const uint16_t numOfStates{static_cast<uint16_t>(vpa->numOfStates + 1)};
    return {
        common::transition::State{static_cast<uint16_t>(state % numOfStates)},
        common::transition::State{static_cast<uint16_t>(state / numOfStates)}};
}

template <AutomatonKind Kind>
std::pair<common::symbol::StackSymbol, common::symbol::StackSymbol> AutomataCombiner<
    Kind>::convertCombinedStackSymbolIntoSymbols(uint16_t stackSymbol) const
{
    if (stackSymbol == common::symbol::StackSymbol::BOTTOM)
    {
        return {common::symbol::StackSymbol::BOTTOM, common::symbol::StackSymbol::BOTTOM};
    }

    return {
        common::symbol::StackSymbol{static_cast<uint16_t>((stackSymbol % (numOfStackSymbols + 1)))},
        common::symbol::StackSymbol{
            static_cast<uint16_t>((stackSymbol / (numOfStackSymbols + 1)))}};
}

template <AutomatonKind Kind>
common::transition::State AutomataCombiner<Kind>::combineStates(uint16_t s1, uint16_t s2) const
{
    return common::transition::State{static_cast<uint16_t>(s2 * (vpa->numOfStates + 1) + s1)};
}

template <AutomatonKind Kind>
common::symbol::StackSymbol AutomataCombiner<Kind>::combineStackSymbols(
    uint16_t s1, uint16_t s2) const
{
    if (s1 == common::symbol::StackSymbol::BOTTOM)
    {
        return common::symbol::StackSymbol::BOTTOM;
    }

    return common::symbol::StackSymbol{static_cast<uint16_t>((s2 * (numOfStackSymbols + 1)) + s1)};
}

template class AutomataCombiner<AutomatonKind::Normal>;
template class AutomataCombiner<AutomatonKind::Combined>;
} // namespace teacher