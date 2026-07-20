#include <memory>

#include "common/transition/State.hpp"
#include "common/transition/Transition.hpp"
#include "learner/srs/SpecialSymbolAdder.hpp"

namespace learner::srs
{
SpecialSymbolAdder::SpecialSymbolAdder(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &hypothesis)
    : numOfStates{hypothesis->getNumOfStates()}
{
    convertedAutomata = std::make_shared<ConvertedAutomata>();

    convertedAutomata->lAutomaton =
        std::make_shared<common::VPA<AutomatonKind::Normal>>(*hypothesis);
    convertedAutomata->rAutomaton =
        std::make_shared<common::VPA<AutomatonKind::Normal>>(*hypothesis);
}

void SpecialSymbolAdder::addNewRule(const uint16_t specialSymbol, const SrsRule &srsRule)
{
    addTransitionsForSpecialSymbol(convertedAutomata->lAutomaton, srsRule.left, specialSymbol);
    addTransitionsForSpecialSymbol(convertedAutomata->rAutomaton, srsRule.right, specialSymbol);
}

void SpecialSymbolAdder::addNewCallRule(const uint16_t specialSymbol, const SrsRule &srsRule)
{
    addCallTransitionsForSpecialSymbol(convertedAutomata->lAutomaton, srsRule.left, specialSymbol);
    addCallTransitionsForSpecialSymbol(convertedAutomata->rAutomaton, srsRule.right, specialSymbol);
}

std::shared_ptr<ConvertedAutomata> SpecialSymbolAdder::getConvertedAutomata()
{
    return convertedAutomata;
}

void SpecialSymbolAdder::addTransitionsForSpecialSymbol(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> &automaton, const common::Word &word,
    const uint16_t specialSymbol)
{
    for (uint16_t stateId = 0; stateId < numOfStates; stateId++)
    {
        common::transition::State state{stateId};
        automaton->state = state;
        automaton->readWord(word);

        automaton->delta->add(state, common::symbol::LocalSymbol{specialSymbol}, automaton->state);
    }
}

void SpecialSymbolAdder::addCallTransitionsForSpecialSymbol(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> &automaton, const common::Word &word,
    const uint16_t specialSymbol)
{
    for (uint16_t stateId = 0; stateId < numOfStates; stateId++)
    {
        const common::transition::State state{stateId};
        automaton->state = state;
        automaton->stack = {};
        automaton->readWord(word);

        automaton->delta->add(
            state, common::symbol::CallSymbol{specialSymbol}, automaton->state,
            automaton->stack.top());
    }
}

} // namespace learner::srs
