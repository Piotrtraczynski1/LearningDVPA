#pragma once

#include <cstdint>

#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"
#include "learner/srs/ConvertedAutomata.hpp"
#include "learner/srs/Srs.hpp"

namespace learner::srs
{
class AutomataConverter
{
    common::symbol::LocalSymbol specialSymbol;
    ConvertedAutomata convertedAutomata;

public:
    AutomataConverter(common::symbol::LocalSymbol specialSymbolArg);

    ConvertedAutomata
    run(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis,
        const SrsRule &srsRule);

private:
    void init(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis);

    void addTransitionsForSpecialSymbol(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> automaton, const common::Word &word,
        const uint16_t numOfStates);
};
} // namespace learner::srs