#pragma once

#include <cstdint>
#include <memory>

#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"
#include "learner/srs/ConvertedAutomata.hpp"
#include "learner/srs/Srs.hpp"

namespace learner::srs
{
class SpecialSymbolAdder
{
    std::shared_ptr<ConvertedAutomata> convertedAutomata;
    const uint16_t numOfStates;

public:
    SpecialSymbolAdder(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &hypothesis);

    void addNewRule(const uint16_t specialSymbol, const SrsRule &srsRule);

    std::shared_ptr<ConvertedAutomata> getConvertedAutomata();

private:
    void init(const std::shared_ptr<common::VPA<AutomatonKind::Normal>> &hypothesis);

    void addTransitionsForSpecialSymbol(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> &automaton, const common::Word &word,
        const uint16_t specialSymbol);
};
} // namespace learner::srs