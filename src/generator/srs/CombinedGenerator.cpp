#include "generator/srs/CombinedGenerator.hpp"
#include "teacher/AutomataCombiner.hpp"
#include "utils/Constants.hpp"
#include "utils/ExitCode.hpp"
#include "utils/log.hpp"

namespace generator::srs
{
void CombinedGenerator::validateGeneratorConfig(
    uint16_t &numOfStates_, [[maybe_unused]] uint16_t &numOfCalls_,
    [[maybe_unused]] uint16_t &numOfLocals_, [[maybe_unused]] uint16_t &numOfReturns_,
    uint16_t &numOfStackSymbols_)
{
    uint16_t adjustedNumOfStates{static_cast<uint16_t>((numOfStates + 1) * (numOfStates + 1))};
    if (adjustedNumOfStates > utils::MaxNumOfAutomatonStates)
    {
        ERR("[CombinedGenerator]: (numOfStates + 1) * (numOfStates + 1) (%u) is greater than "
            "MaxNumOfAutomatonStates!",
            adjustedNumOfStates);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CombinedGenerator] adjusting numOfStates to %u", adjustedNumOfStates);
    numOfStates_ = adjustedNumOfStates;

    uint16_t adjustedNumOfStackSymbols{
        static_cast<uint16_t>((numOfStackSymbols + 1) * (numOfStackSymbols + 1))};
    if (adjustedNumOfStackSymbols > utils::MaxNumOfStackSymbols)
    {
        ERR("[CombinedGenerator]: (numOfStackSymbols + 1) * (numOfStackSymbols + 1) (%u) is "
            "greater than "
            "MaxNumOfStackSymbols!",
            adjustedNumOfStates);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CombinedGenerator] adjusting numOfStackSymbols to %u", adjustedNumOfStackSymbols);
    numOfStackSymbols_ = adjustedNumOfStackSymbols;
}

bool CombinedGenerator::generatorSpecificCheck(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    return (numOfStates + 1) * (numOfStates + 1) >= hypothesis->getNumOfStates();
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> CombinedGenerator::run()
{
    splitAplhabet();

    auto firstVpa =
        generateVpa([&](const common::Symbol &symbol) { return firstVpaPredicate(symbol); });
    auto secondVpa =
        generateVpa([&](const common::Symbol &symbol) { return secondVpaPredicate(symbol); });

    teacher::AutomataCombiner<AutomatonKind::Normal> combiner{
        firstVpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols};
    return combiner.combineVPA(*secondVpa);
}

void CombinedGenerator::splitAplhabet()
{
    callSplitPoint = (rand() % (numOfCalls - 1)) + 1;
    returnSplitPoint = (rand() % (numOfReturns - 1)) + 1;
    localSplitPoint = (rand() % (numOfLocals - 1)) + 1;
}

template <typename Predicate>
std::shared_ptr<common::VPA<AutomatonKind::Normal>> CombinedGenerator::generateVpa(
    const Predicate &isVoidSymbol)
{
    generateTransition(isVoidSymbol);

    acceptingStates.clear();
    selectAcceptingStates();
    return std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, acceptingStates, numOfStates);
}

template <typename Predicate>
void CombinedGenerator::generateTransition(const Predicate &isVoidSymbol)
{
    transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

    for (uint16_t stateId = 0; stateId < numOfStates; stateId++)
    {
        common::transition::State state{stateId};
        addCalls(state, isVoidSymbol);
        addLocals(state, isVoidSymbol);
        addReturns(state, isVoidSymbol);
    }
}

template <typename Predicate>
void CombinedGenerator::addCalls(common::transition::State state, const Predicate &isVoidSymbol)
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
        common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};
        common::symbol::StackSymbol stackSymbol{
            static_cast<uint16_t>((rand() % (numOfStackSymbols - 1)) + 1)};

        transition->add(state, common::symbol::CallSymbol{call}, dest, stackSymbol);
    }
}

template <typename Predicate>
void CombinedGenerator::addLocals(common::transition::State state, const Predicate &isVoidSymbol)
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
        common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};

        transition->add(state, common::symbol::LocalSymbol{local}, dest);
    }
}

template <typename Predicate>
void CombinedGenerator::addReturns(common::transition::State state, const Predicate &isVoidSymbol)
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
            common::transition::State dest{static_cast<uint16_t>(rand() % numOfStates)};
            common::symbol::StackSymbol stackSymbol{stackSymbolId};

            transition->add(state, stackSymbol, common::symbol::ReturnSymbol{ret}, dest);
        }
    }
}

void CombinedGenerator::addVoidCall(const common::transition::State state, const uint16_t callId)
{
    transition->add(
        state, common::symbol::CallSymbol{callId}, state, common::symbol::StackSymbol{1});
}

void CombinedGenerator::addVoidReturn(
    const common::transition::State state, const uint16_t stackSymbolId, const uint16_t returnId)
{
    transition->add(
        state, common::symbol::StackSymbol{stackSymbolId}, common::symbol::ReturnSymbol{returnId},
        state);
}

void CombinedGenerator::addVoidLocal(const common::transition::State state, const uint16_t localId)
{
    transition->add(state, common::symbol::LocalSymbol{localId}, state);
}

bool CombinedGenerator::firstVpaPredicate(const common::Symbol &symbol)
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
    ERR("[CombinedGenerator] invalid symbol!");
    return false;
}

bool CombinedGenerator::secondVpaPredicate(const common::Symbol &symbol)
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
    ERR("[CombinedGenerator] invalid symbol!");
    return false;
}

learner::srs::Srs CombinedGenerator::getSrs()
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