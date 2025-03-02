#include <gtest/gtest.h>
#include <map>
#include <stdexcept>
#include <string>

#include "../../../common/Stack.cpp"
#include "../../../common/VPA.cpp"
#include "../../../common/transition/Transition.cpp"
#include "../Parser.cpp"

using namespace common;
using namespace common::symbol;
using namespace common::transition;

namespace mainComponent::parser
{
class TestParser : public ::testing::Test
{
public:
    void SetUp() override
    {
        path = path.substr(0, path.find_last_of("/\\") + 1);
        path += "../inputs/input.json";
        sut = parser.readVPA(path);
    }

    Parser parser;
    std::string path = std::string(__FILE__);
    std::shared_ptr<common::VPA> sut;

    // poprawne nawiasowania:
    std::vector<Symbol> word1{CallSymbol{0}, ReturnSymbol{0}};
    std::vector<Symbol> word2{CallSymbol{0},   CallSymbol{0},   CallSymbol{0}, ReturnSymbol{0},
                              ReturnSymbol{0}, ReturnSymbol{0}, CallSymbol{0}, ReturnSymbol{0},
                              CallSymbol{0},   ReturnSymbol{0}, CallSymbol{0}, ReturnSymbol{0}};

    std::vector<Symbol> word3{LocalSymbol{0},  LocalSymbol{1}, CallSymbol{0}, LocalSymbol{0},
                              ReturnSymbol{0}, LocalSymbol{1}, LocalSymbol{0}};

    // niepoprawne nawiasowania:
    std::vector<Symbol> word4{LocalSymbol{0}, LocalSymbol{1}, CallSymbol{0}};
    std::vector<Symbol> word5{LocalSymbol{0}, LocalSymbol{1},  CallSymbol{0},  CallSymbol{0},
                              CallSymbol{0},  ReturnSymbol{0}, ReturnSymbol{0}};

    // Wyjatek
    std::vector<Symbol> word6{CallSymbol{1}};
    std::vector<Symbol> word7{LocalSymbol{0},  LocalSymbol{1}, CallSymbol{0},
                              ReturnSymbol{0}, LocalSymbol{1}, ReturnSymbol{0}};
    std::vector<Symbol> word8{LocalSymbol{0},  LocalSymbol{1}, CallSymbol{0}, LocalSymbol{0},
                              ReturnSymbol{0}, LocalSymbol{1}, LocalSymbol{2}};
};

TEST_F(TestParser, default1) { EXPECT_EQ(sut->checkWord(word1), true); }
TEST_F(TestParser, default2) { EXPECT_EQ(sut->checkWord(word2), true); }
TEST_F(TestParser, default3) { EXPECT_EQ(sut->checkWord(word3), true); }

TEST_F(TestParser, default4) { EXPECT_EQ(sut->checkWord(word4), false); }
TEST_F(TestParser, default5) { EXPECT_EQ(sut->checkWord(word5), false); }

TEST_F(TestParser, default6) { EXPECT_THROW(sut->checkWord(word6), std::out_of_range); }
TEST_F(TestParser, default7) { EXPECT_THROW(sut->checkWord(word7), std::out_of_range); }
TEST_F(TestParser, default8) { EXPECT_THROW(sut->checkWord(word8), std::out_of_range); }

TEST_F(TestParser, default9)
{
    std::string testWord{"(())"};
    Word word{parser.parseString(testWord)};
    EXPECT_EQ(sut->checkWord(word), true);
};

TEST_F(TestParser, default10)
{
    std::string testWord{"a()a((a)b)b((()))baab(b)b(a)"};
    Word word{parser.parseString(testWord)};
    EXPECT_EQ(sut->checkWord(word), true);
};

TEST_F(TestParser, default11)
{
    std::string testWord{"a()a((a)b)b((()))baab(b)b(a)("};
    Word word{parser.parseString(testWord)};
    EXPECT_EQ(sut->checkWord(word), false);
};
} // namespace mainComponent::parser
