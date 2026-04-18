#include "generator/srs/CommutativeGenerator.hpp"
#include "teacher/AutomataCombiner.hpp"
#include "utils/Constants.hpp"
#include "utils/ExitCode.hpp"
#include "utils/log.hpp"

namespace generator::srs
{
void CommutativeGenerator::validateGeneratorConfig(
    uint16_t &numOfStates_, [[maybe_unused]] uint16_t &numOfCalls_,
    [[maybe_unused]] uint16_t &numOfLocals_, [[maybe_unused]] uint16_t &numOfReturns_,
    uint16_t &numOfStackSymbols_)
{
    uint16_t adjustedNumOfStates{
        static_cast<uint16_t>((numOfStates + 1) * (secondDvpaNumOfStates + 1))};
    if (adjustedNumOfStates > utils::MaxNumOfAutomatonStates)
    {
        ERR("[CommutativeGenerator]: (numOfStates + 1) * (secondDvpaNumOfStates + 1) (%u) is "
            "greater than "
            "MaxNumOfAutomatonStates!",
            adjustedNumOfStates);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CommutativeGenerator] adjusting numOfStates to %u", adjustedNumOfStates);
    numOfStates_ = adjustedNumOfStates;

    uint16_t adjustedNumOfStackSymbols{
        static_cast<uint16_t>((numOfStackSymbols + 1) * (numOfStackSymbols + 1))};
    if (adjustedNumOfStackSymbols > utils::MaxNumOfStackSymbols)
    {
        ERR("[CommutativeGenerator]: (numOfStackSymbols + 1) * (numOfStackSymbols + 1) (%u) is "
            "greater than "
            "MaxNumOfStackSymbols!",
            adjustedNumOfStates);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CommutativeGenerator] adjusting numOfStackSymbols to %u", adjustedNumOfStackSymbols);
    numOfStackSymbols_ = adjustedNumOfStackSymbols;
}

bool CommutativeGenerator::generatorSpecificCheck(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    return (numOfStates + 1) * (secondDvpaNumOfStates + 1) >= hypothesis->getNumOfStates();
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> CommutativeGenerator::run()
{
    splitAplhabet();

    auto firstVpa = generateVpa(
        [&](const common::Symbol &symbol) { return firstVpaPredicate(symbol); }, numOfStates);
    auto secondVpa = generateVpa(
        [&](const common::Symbol &symbol) { return secondVpaPredicate(symbol); },
        secondDvpaNumOfStates);

    teacher::AutomataCombiner<AutomatonKind::Normal> combiner{
        firstVpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols};
    return combiner.combineVPA(*secondVpa);
}

void CommutativeGenerator::splitAplhabet()
{
    callSplitPoint = (rand() % (numOfCalls - 1)) + 1;
    returnSplitPoint = (rand() % (numOfReturns - 1)) + 1;
    localSplitPoint = (rand() % (numOfLocals - 1)) + 1;
}

template <typename Predicate>
std::shared_ptr<common::VPA<AutomatonKind::Normal>> CommutativeGenerator::generateVpa(
    const Predicate &isVoidSymbol, const uint16_t specificVpaNumOfStates)
{
    generateTransition(isVoidSymbol, specificVpaNumOfStates);

    acceptingStates.clear();
    selectAcceptingStates(specificVpaNumOfStates);
    return std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, acceptingStates, specificVpaNumOfStates);
}

template <typename Predicate>
void CommutativeGenerator::generateTransition(
    const Predicate &isVoidSymbol, const uint16_t specificVpaNumOfStates)
{
    transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

    for (uint16_t stateId = 0; stateId < specificVpaNumOfStates; stateId++)
    {
        common::transition::State state{stateId};
        addCalls(state, isVoidSymbol, specificVpaNumOfStates);
        addLocals(state, isVoidSymbol, specificVpaNumOfStates);
        addReturns(state, isVoidSymbol, specificVpaNumOfStates);
    }
}

template <typename Predicate>
void CommutativeGenerator::addCalls(
    common::transition::State state, const Predicate &isVoidSymbol,
    const uint16_t specificVpaNumOfStates)
{
    for (uint16_t call = 0; call < numOfCalls; call++)
    {
        if (isVoidSymbol(common::Symbol{common::symbol::CallSymbol{call}}))
        {
            addVoidCall(state, call);
            continue;
        }

        if (skipTransition())
        {
            continue;
        }
        common::transition::State dest{static_cast<uint16_t>(rand() % specificVpaNumOfStates)};
        common::symbol::StackSymbol stackSymbol{
            static_cast<uint16_t>((rand() % (numOfStackSymbols - 1)) + 1)};

        transition->add(state, common::symbol::CallSymbol{call}, dest, stackSymbol);
    }
}

template <typename Predicate>
void CommutativeGenerator::addLocals(
    common::transition::State state, const Predicate &isVoidSymbol,
    const uint16_t specificVpaNumOfStates)
{
    for (uint16_t local = 0; local < numOfLocals; local++)
    {
        if (isVoidSymbol(common::Symbol{common::symbol::LocalSymbol{local}}))
        {
            addVoidLocal(state, local);
            continue;
        }

        if (skipTransition())
        {
            continue;
        }
        common::transition::State dest{static_cast<uint16_t>(rand() % specificVpaNumOfStates)};

        transition->add(state, common::symbol::LocalSymbol{local}, dest);
    }
}

template <typename Predicate>
void CommutativeGenerator::addReturns(
    common::transition::State state, const Predicate &isVoidSymbol,
    const uint16_t specificVpaNumOfStates)
{
    for (uint16_t ret = 0; ret < numOfReturns; ret++)
    {
        for (uint16_t stackSymbolId = 0; stackSymbolId < numOfStackSymbols; stackSymbolId++)
        {
            if (isVoidSymbol(common::Symbol{common::symbol::ReturnSymbol{ret}}))
            {
                addVoidReturn(state, stackSymbolId, ret);
                continue;
            }

            if (skipTransition())
            {
                continue;
            }
            common::transition::State dest{static_cast<uint16_t>(rand() % specificVpaNumOfStates)};
            common::symbol::StackSymbol stackSymbol{stackSymbolId};

            transition->add(state, stackSymbol, common::symbol::ReturnSymbol{ret}, dest);
        }
    }
}

void CommutativeGenerator::addVoidCall(const common::transition::State state, const uint16_t callId)
{
    transition->add(
        state, common::symbol::CallSymbol{callId}, state, common::symbol::StackSymbol{1});
}

void CommutativeGenerator::addVoidReturn(
    const common::transition::State state, const uint16_t stackSymbolId, const uint16_t returnId)
{
    transition->add(
        state, common::symbol::StackSymbol{stackSymbolId}, common::symbol::ReturnSymbol{returnId},
        state);
}

void CommutativeGenerator::addVoidLocal(
    const common::transition::State state, const uint16_t localId)
{
    transition->add(state, common::symbol::LocalSymbol{localId}, state);
}

bool CommutativeGenerator::firstVpaPredicate(const common::Symbol &symbol)
{
    switch (symbol.index())
    {
    case common::CallSymbolVariant:
    {
        auto callId{static_cast<uint16_t>(std::get<common::symbol::CallSymbol>(symbol))};
        return callId < callSplitPoint;
    }
    case common::ReturnSymbolVariant:
    {
        auto returnId{static_cast<uint16_t>(std::get<common::symbol::ReturnSymbol>(symbol))};
        return returnId < returnSplitPoint;
    }
    case common::LocalSymbolVariant:
    {
        auto localId{static_cast<uint16_t>(std::get<common::symbol::LocalSymbol>(symbol))};
        return localId < localSplitPoint;
    }
    }
    ERR("[CommutativeGenerator] invalid symbol!");
    return false;
}

bool CommutativeGenerator::secondVpaPredicate(const common::Symbol &symbol)
{
    switch (symbol.index())
    {
    case common::CallSymbolVariant:
    {
        auto callId{static_cast<uint16_t>(std::get<common::symbol::CallSymbol>(symbol))};
        return callId >= callSplitPoint;
    }
    case common::ReturnSymbolVariant:
    {
        auto returnId{static_cast<uint16_t>(std::get<common::symbol::ReturnSymbol>(symbol))};
        return returnId >= returnSplitPoint;
    }
    case common::LocalSymbolVariant:
    {
        auto localId{static_cast<uint16_t>(std::get<common::symbol::LocalSymbol>(symbol))};
        return localId >= localSplitPoint;
    }
    }
    ERR("[CommutativeGenerator] invalid symbol!");
    return false;
}

void CommutativeGenerator::selectAcceptingStates(const uint16_t specificVpaNumOfStates)
{
    for (uint16_t i = 0; i < specificVpaNumOfStates; i++)
    {
        if (shouldAccept())
        {
            acceptingStates.push_back(i);
        }
    }
}

learner::srs::Srs CommutativeGenerator::getSrs()
{
    learner::srs::Srs srs{};

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