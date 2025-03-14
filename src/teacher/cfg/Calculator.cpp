#include <cstdint>
#include <optional>
#include <vector>

#include "teacher/cfg/Calculator.hpp"

namespace teacher::cfg::Calculator
{

namespace
{
constexpr uint64_t minCallSymbol = 1ULL << 34;
constexpr uint64_t minRetrunSymbl = 1ULL << 17;
} // namespace

NonTerminal makeNonTerminal(
    const common::transition::State q1, const common::symbol::StackSymbol s,
    const common::transition::State q2)
{
    uint64_t firstStateId = static_cast<uint64_t>(q1 + 1) << 34;
    uint64_t stackId = static_cast<uint64_t>(s + 1) << 17;
    uint64_t secondStateId = static_cast<uint64_t>(q2 + 1);

    return cfg::NonTerminal{firstStateId + stackId + secondStateId};
}

common::Symbol convertTerminalToSymbol(const Terminal terminal)
{
    uint64_t val = static_cast<uint64_t>(terminal);
    if (val >= minCallSymbol)
    {
        val = (val >> 34) - 1;
        return common::symbol::CallSymbol{static_cast<uint16_t>(val)};
    }
    if (val >= minRetrunSymbl)
    {
        val = (val >> 17) - 1;
        return common::symbol::ReturnSymbol{static_cast<uint16_t>(val)};
    }
    return common::symbol::LocalSymbol{static_cast<uint16_t>(val - 1)};
}

Terminal convertSymbolToTerminal(const common::Symbol symbol)
{
    switch (symbol.index())
    {
    case 0:
    {
        auto callSymbol = std::get<common::symbol::CallSymbol>(symbol);
        uint64_t terminal = static_cast<uint64_t>(callSymbol + 1) << 34;
        return cfg::Terminal{terminal};
    }
    case 1:
    {
        auto returnSymbol = std::get<common::symbol::ReturnSymbol>(symbol);
        uint64_t terminal = static_cast<uint64_t>(returnSymbol + 1) << 17;
        return cfg::Terminal{terminal};
    }
    case 2:
    {
        auto localSymbol = std::get<common::symbol::LocalSymbol>(symbol);
        uint64_t terminal = static_cast<uint64_t>(localSymbol + 1);
        return cfg::Terminal{terminal};
    }
    default:
    {
        throw std::invalid_argument("Tring to conver control word");
        return cfg::Terminal{0};
    }
    }
}

std::shared_ptr<common::Word>
convertCfgOutputToWord(const std::optional<std::vector<Terminal>> &output)
{
    std::shared_ptr<common::Word> word = std::make_shared<common::Word>();
    if (not output.has_value())
    {
        return word;
    }

    for (auto terminal : output.value())
    {
        *word += cfg::Calculator::convertTerminalToSymbol(terminal);
    }
    return word;
}
} // namespace teacher::cfg::Calculator
