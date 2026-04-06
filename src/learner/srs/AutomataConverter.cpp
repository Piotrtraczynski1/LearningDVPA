#include <memory>

#include "common/transition/State.hpp"
#include "common/transition/Transition.hpp"
#include "learner/srs/AutomataConverter.hpp"

namespace learner::srs
{
AutomataConverter::AutomataConverter(common::symbol::LocalSymbol specialSymbolArg)
    : specialSymbol{specialSymbolArg}
{
}

ConvertedAutomata AutomataConverter::run(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &hypothesis, const SrsRule &srsRule)
{
    init(hypothesis);
    const auto numOfStates{hypothesis->getNumOfStates()};

    addTransitionsForSpecialSymbol(convertedAutomata.lAutomaton, srsRule.left, numOfStates);
    addTransitionsForSpecialSymbol(convertedAutomata.rAutomaton, srsRule.right, numOfStates);

    return convertedAutomata;
}

void AutomataConverter::init(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &hypothesis)
{
    convertedAutomata.lAutomaton =
        std::make_shared<common::VPA<AutomatonKind::Normal>>(*hypothesis);

    convertedAutomata.rAutomaton =
        std::make_shared<common::VPA<AutomatonKind::Normal>>(*hypothesis);
}

void AutomataConverter::addTransitionsForSpecialSymbol(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> &automaton, const common::Word &word,
    const uint16_t numOfStates)
{
    for (uint16_t stateId = 0; stateId < numOfStates; stateId++)
    {
        common::transition::State state{stateId};
        automaton->state = state;
        automaton->readWord(word);

        automaton->delta->add(state, specialSymbol, automaton->state);
    }
}

} // namespace learner::srs