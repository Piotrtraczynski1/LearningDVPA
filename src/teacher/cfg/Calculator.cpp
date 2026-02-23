#include <cstdint>
#include <optional>
#include <vector>

#include "teacher/cfg/Calculator.hpp"
#include "utils/Constants.hpp"
#include "utils/log.hpp"

namespace teacher::cfg::Calculator
{

namespace
{
constexpr uint32_t maxNumOfLetters{utils::MaxNumOfLetters + 2};
} // namespace

NonTerminal makeNonTerminal(
    const common::transition::State q1, const common::symbol::StackSymbol s,
    const common::transition::State q2)
{
    return NonTerminal{static_cast<uint32_t>((q1 * utils::S + s) * utils::Q + q2)};
}

std::tuple<common::transition::State, common::symbol::StackSymbol, common::transition::State>
decodeNonTerminal(const NonTerminal nt)
{
    const uint32_t id = static_cast<uint32_t>(nt);
    const uint32_t tmp{id / utils::Q};

    return {
        common::transition::State{static_cast<uint16_t>(tmp / utils::S)},
        common::symbol::StackSymbol{static_cast<uint16_t>(tmp % utils::S)},
        common::transition::State{static_cast<uint16_t>(id % utils::Q)}};
}

common::Symbol convertTerminalToSymbol(const Terminal terminal)
{
    uint32_t val = static_cast<uint32_t>(terminal);
    if (val >= maxNumOfLetters * maxNumOfLetters)
    {
        val /= maxNumOfLetters * maxNumOfLetters;
        return common::symbol::CallSymbol{static_cast<uint16_t>(val - 1)};
    }
    if (val >= maxNumOfLetters)
    {
        val /= maxNumOfLetters;
        return common::symbol::ReturnSymbol{static_cast<uint16_t>(val - 1)};
    }
    return common::symbol::LocalSymbol{static_cast<uint16_t>(val)};
}

Terminal convertSymbolToTerminal(const common::Symbol symbol)
{
    switch (symbol.index())
    {
    case common::CallSymbolVariant:
    {
        auto callSymbol = std::get<common::symbol::CallSymbol>(symbol);
        uint32_t terminal =
            static_cast<uint32_t>((callSymbol + 1) * maxNumOfLetters * maxNumOfLetters);
        return cfg::Terminal{terminal};
    }
    case common::ReturnSymbolVariant:
    {
        auto returnSymbol = std::get<common::symbol::ReturnSymbol>(symbol);
        uint32_t terminal = static_cast<uint32_t>((returnSymbol + 1) * maxNumOfLetters);
        return cfg::Terminal{terminal};
    }
    case common::LocalSymbolVariant:
    {
        auto localSymbol = std::get<common::symbol::LocalSymbol>(symbol);
        uint32_t terminal = static_cast<uint32_t>(localSymbol);
        return cfg::Terminal{terminal};
    }
    default:
    {
        ERR("[Calculator]: Tring to convert control word");
        return cfg::Terminal::INVALID;
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
