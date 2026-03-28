#include <gtest/gtest.h>

#include "teacher/cfg/Cfg.hpp"

using namespace teacher::cfg;

TEST(CfgTest, default1)
{
    Cfg cfg{1000, 100};

    cfg.addProduction(NonTerminal::START, Terminal{1}, NonTerminal{1}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{1}, Terminal{2}, NonTerminal{1}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{1}, Terminal{3}, NonTerminal::INVALID, NonTerminal::INVALID);

    auto result = cfg.isEmpty();

    EXPECT_TRUE((*result).has_value());
    EXPECT_FALSE((*result)->empty());

    std::vector<Terminal> expectedPrefix = {Terminal{1}, Terminal{2}, Terminal{3}};
    EXPECT_EQ((*result)->front(), expectedPrefix.front());
}

TEST(CfgTest, EmptyGrammar)
{
    Cfg cfg;

    auto result = cfg.isEmpty();

    EXPECT_FALSE((*result).has_value());
}

TEST(CfgTest, cyclicGrammar)
{
    Cfg cfg;

    cfg.addProduction(NonTerminal::START, Terminal{1}, NonTerminal{1}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{1}, Terminal{2}, NonTerminal{2}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{2}, Terminal{1}, NonTerminal{2}, NonTerminal::INVALID);

    auto result = cfg.isEmpty();

    EXPECT_FALSE((*result).has_value());
}

TEST(CfgTest, complexNonEmptyGrammar)
{
    Cfg cfg;

    cfg.addProduction(NonTerminal::START, Terminal{1}, NonTerminal{1}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{1}, Terminal{2}, NonTerminal{2}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{2}, Terminal{1}, NonTerminal{2}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{2}, Terminal{3}, NonTerminal::INVALID, NonTerminal::INVALID);

    auto result = cfg.isEmpty();

    std::vector<Terminal> expectedPrefix = {Terminal{1}, Terminal{2}, Terminal{3}};
    EXPECT_EQ((*result)->front(), expectedPrefix.front());
}

TEST(CfgTest, test)
{
    Cfg cfg;

    cfg.addProduction(NonTerminal::START, Terminal::INVALID, NonTerminal{1}, NonTerminal::INVALID);

    cfg.addProduction(NonTerminal{1}, Terminal{1}, NonTerminal{2}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{2}, Terminal{1}, NonTerminal{2}, NonTerminal::INVALID);
    cfg.addProduction(NonTerminal{2}, Terminal{3}, NonTerminal::INVALID, NonTerminal::INVALID);

    auto result = cfg.isEmpty();

    std::vector<Terminal> expectedPrefix = {Terminal{1}, Terminal{3}};
    EXPECT_EQ((*result)->front(), expectedPrefix.front());
}
