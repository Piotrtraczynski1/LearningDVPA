#include <gtest/gtest.h>

#include "common/Stack.cpp"
#include "common/Word.cpp"
#include "teacher/cfg/Calculator.cpp"
#include "utils/TimeMarker.cpp"

#include "teacher/cfg/Cfg.cpp"

using namespace teacher::cfg;

TEST(CfgTest, default1)
{
    Cfg cfg;

    cfg.addProjection(START, Terminal{1}, NonTerminal{1}, std::nullopt);
    cfg.addProjection(NonTerminal{1}, Terminal{2}, NonTerminal{1}, std::nullopt);
    cfg.addProjection(NonTerminal{1}, Terminal{3}, std::nullopt, std::nullopt);

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

    cfg.addProjection(START, Terminal{1}, NonTerminal{1}, std::nullopt);
    cfg.addProjection(NonTerminal{1}, Terminal{2}, NonTerminal{2}, std::nullopt);
    cfg.addProjection(NonTerminal{2}, Terminal{1}, NonTerminal{2}, std::nullopt);

    auto result = cfg.isEmpty();

    EXPECT_FALSE((*result).has_value());
}

TEST(CfgTest, complexNonEmptyGrammar)
{
    Cfg cfg;

    cfg.addProjection(START, Terminal{1}, NonTerminal{1}, std::nullopt);
    cfg.addProjection(NonTerminal{1}, Terminal{2}, NonTerminal{2}, std::nullopt);
    cfg.addProjection(NonTerminal{2}, Terminal{1}, NonTerminal{2}, std::nullopt);
    cfg.addProjection(NonTerminal{2}, Terminal{3}, std::nullopt, std::nullopt);

    auto result = cfg.isEmpty();

    std::vector<Terminal> expectedPrefix = {Terminal{1}, Terminal{2}, Terminal{3}};
    EXPECT_EQ((*result)->front(), expectedPrefix.front());
}

TEST(CfgTest, test)
{
    Cfg cfg;

    cfg.addProjection(START, std::nullopt, NonTerminal{1}, std::nullopt);

    cfg.addProjection(NonTerminal{1}, Terminal{1}, NonTerminal{2}, std::nullopt);
    cfg.addProjection(NonTerminal{2}, Terminal{1}, NonTerminal{2}, std::nullopt);
    cfg.addProjection(NonTerminal{2}, Terminal{3}, std::nullopt, std::nullopt);

    auto result = cfg.isEmpty();

    std::vector<Terminal> expectedPrefix = {Terminal{1}, Terminal{3}};
    EXPECT_EQ((*result)->front(), expectedPrefix.front());
}
