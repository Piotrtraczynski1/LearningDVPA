#include "generator/srs/CommentsGenerator.hpp"
#include "utils/Constants.hpp"
#include "utils/ExitCode.hpp"
#include "utils/log.hpp"

namespace generator::srs
{
void CommentsGenerator::validateGeneratorConfig(
    uint16_t &numOfStates_, uint16_t &numOfCalls_, [[maybe_unused]] uint16_t &numOfLocals_,
    uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_)
{
    uint16_t adjustedNumOfStates{numOfStates * 2};
    if (adjustedNumOfStates > utils::MaxNumOfAutomatonStates)
    {
        ERR("[CombinedGenerator]: numOfStates * 2 (%u) is greater than "
            "MaxNumOfAutomatonStates!",
            adjustedNumOfStates);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CombinedGenerator] adjusting numOfStates to %u", adjustedNumOfStates);
    numOfStates_ = adjustedNumOfStates;

    uint16_t adjustedNumOfStackSymbols{numOfStackSymbols + 1};
    if (adjustedNumOfStackSymbols > utils::MaxNumOfStackSymbols)
    {
        ERR("[CombinedGenerator]: numOfStackSymbols + 1 (%u) is "
            "greater than "
            "MaxNumOfStackSymbols!",
            adjustedNumOfStates);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CombinedGenerator] adjusting numOfStackSymbols to %u", adjustedNumOfStackSymbols);
    numOfStackSymbols_ = adjustedNumOfStackSymbols;

    uint16_t adjustedNumOfCalls{numOfCalls + 1};
    if (adjustedNumOfCalls > utils::MaxNumOfLetters)
    {
        ERR("[CombinedGenerator]: numOfCalls + 1 (%u) is "
            "greater than "
            "MaxNumOfLetters!",
            adjustedNumOfCalls);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CombinedGenerator] adjusting numOfCalls to %u", adjustedNumOfCalls);
    numOfCalls_ = adjustedNumOfCalls;

    uint16_t adjustedNumOfReturns{numOfReturns + 1};
    if (adjustedNumOfReturns > utils::MaxNumOfLetters)
    {
        ERR("[CombinedGenerator]: numOfReturns + 1 (%u) is "
            "greater than "
            "MaxNumOfLetters!",
            adjustedNumOfReturns);
        exit(toExit(ExitCode::GENERATOR));
    }

    LOG("[CombinedGenerator] adjusting numOfReturns to %u", adjustedNumOfReturns);
    numOfReturns_ = adjustedNumOfReturns;
}

bool CommentsGenerator::generatorSpecificCheck(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    return numOfStates * 2 + 1 >= hypothesis->getNumOfStates();
}

learner::srs::Srs CommentsGenerator::getSrs()
{
    return learner::srs::Srs{learner::srs::SrsRuleTmp{
        .left =
            {.left = common::Word{common::symbol::CallSymbol{numOfCalls}},
             .right = common::Word{common::symbol::ReturnSymbol{numOfReturns}}},
        .right = {.takesParams = false}}};
}

std::shared_ptr<common::VPA<AutomatonKind::Normal>> CommentsGenerator::run()
{
    generateTransition();
    addComments();
    selectAcceptingStates();

    return std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, acceptingStates, numOfStates * 2);
}

void CommentsGenerator::addComments()
{
    const common::symbol::CallSymbol startComment{numOfCalls};
    const common::symbol::StackSymbol stackSymbolComment{numOfStackSymbols};
    const common::symbol::ReturnSymbol endComment{numOfReturns};
    for (uint16_t stateId = 0; stateId < numOfStates; stateId++)
    {
        common::transition::State commentState{numOfStates + stateId};

        transition->add(
            common::transition::State{stateId}, startComment, commentState, stackSymbolComment);
        transition->add(
            commentState, stackSymbolComment, endComment, common::transition::State{stateId});

        for (uint16_t localId = 0; localId < numOfLocals; localId++)
        {
            transition->add(commentState, common::symbol::LocalSymbol{localId}, commentState);
        }

        for (uint16_t callId = 0; callId <= numOfCalls; callId++)
        {
            transition->add(
                commentState, common::symbol::CallSymbol{callId}, commentState,
                common::symbol::StackSymbol{1});
        }

        for (uint16_t returnId = 0; returnId <= numOfReturns; returnId++)
        {
            transition->add(
                commentState, common::symbol::StackSymbol{1},
                common::symbol::ReturnSymbol{returnId}, commentState);
        }
    }
}
} // namespace generator::srs