#include <memory>
#include <optional>

#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"
#include "common/transition/State.hpp"
#include "teacher/cfg/NonTerminal.hpp"
#include "teacher/cfg/Terminal.hpp"

namespace teacher::cfg::Calculator
{
NonTerminal makeNonTerminal(
    const common::transition::State q1, const common::symbol::StackSymbol s,
    const common::transition::State q2);
common::Symbol convertTerminalToSymbol(const Terminal terminal);
Terminal convertSymbolToTerminal(const common::Symbol symbol);
std::shared_ptr<common::Word>
convertCfgOutputToWord(const std::optional<std::vector<Terminal>> &output);
} // namespace teacher::cfg::Calculator