#include <gtest/gtest.h>

#include "common/Stack.cpp"
#include "common/Word.cpp"

#include "teacher/cfg/Calculator.cpp"

namespace teacher::cfg::Calculator
{

std::vector<Terminal> convertWordToTerminals(common::Word &word)
{
    std::vector<cfg::Terminal> terminals;

    for (auto symbol : word)
    {
        terminals.push_back(cfg::Calculator::convertSymbolToTerminal(symbol));
    }
    return terminals;
}

TEST(Calculator, convertCfgOutputToWord)
{
    common::Word word{common::symbol::CallSymbol{0},      common::symbol::CallSymbol{623},
                      common::symbol::CallSymbol{0xffff}, common::symbol::ReturnSymbol{0},
                      common::symbol::ReturnSymbol{42},   common::symbol::ReturnSymbol{0xffff},
                      common::symbol::LocalSymbol{0},     common::symbol::LocalSymbol{1623},
                      common::symbol::LocalSymbol{0xffff}};
    std::optional<std::vector<Terminal>> cfgOutput{convertWordToTerminals(word)};
    EXPECT_EQ(word, *convertCfgOutputToWord(cfgOutput));
}
} // namespace teacher::cfg::Calculator