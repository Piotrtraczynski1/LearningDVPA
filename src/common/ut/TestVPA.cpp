#include <gtest/gtest.h>
#include <map>
#include <stack>
#include <stdexcept>

#include "common/Stack.cpp"
#include "common/Word.cpp"
#include "common/transition/Transition.cpp"

#include "common/VPA.cpp"

using namespace common::symbol;

namespace common::transition
{
class TestVPA : public ::testing::Test
{
public:
    void SetUp() override
    {
        transition = new Transition{};

        transition->add(initialState, callSymbol, secondState, stackSymbol_1);
        transition->add(secondState, callSymbol, secondState, stackSymbol_2);

        transition->add(initialState, localSymbol_1, initialState);
        transition->add(secondState, localSymbol_1, secondState);

        transition->add(initialState, localSymbol_2, initialState);
        transition->add(secondState, localSymbol_2, secondState);

        transition->add(secondState, stackSymbol_1, returnSymbol, initialState);
        transition->add(secondState, stackSymbol_2, returnSymbol, secondState);

        sut = new VPA{*transition, initialState, {1}, 2};
    }

    State initialState{1};
    State secondState{2};

    CallSymbol callSymbol{1};
    LocalSymbol localSymbol_1{1};
    LocalSymbol localSymbol_2{2};
    ReturnSymbol returnSymbol{1};

    StackSymbol stackSymbol_1{1};
    StackSymbol stackSymbol_2{2};

    VPA *sut;
    Transition *transition;

    // expect true
    Word word1{CallSymbol{1}, ReturnSymbol{1}};
    Word word2{CallSymbol{1},   CallSymbol{1},   CallSymbol{1}, ReturnSymbol{1},
               ReturnSymbol{1}, ReturnSymbol{1}, CallSymbol{1}, ReturnSymbol{1},
               CallSymbol{1},   ReturnSymbol{1}, CallSymbol{1}, ReturnSymbol{1}};

    Word word3{LocalSymbol{1},  LocalSymbol{2}, CallSymbol{1}, LocalSymbol{1},
               ReturnSymbol{1}, LocalSymbol{2}, LocalSymbol{1}};

    // expect false
    Word word4{LocalSymbol{1}, LocalSymbol{2}, CallSymbol{1}};
    Word word5{LocalSymbol{1}, LocalSymbol{2},  CallSymbol{1},  CallSymbol{1},
               CallSymbol{1},  ReturnSymbol{1}, ReturnSymbol{1}};

    // unknown symbol
    Word word6{CallSymbol{2}};
    Word word7{LocalSymbol{1},  LocalSymbol{2}, CallSymbol{1},
               ReturnSymbol{1}, LocalSymbol{2}, ReturnSymbol{1}};
    Word word8{LocalSymbol{1},  LocalSymbol{2}, CallSymbol{1}, LocalSymbol{1},
               ReturnSymbol{1}, LocalSymbol{2}, LocalSymbol{3}};

    // Control Words
    Word word9{CallSymbol{1}, CallSymbol{1},         CallSymbol{1},  CallSymbol{1},
               CallSymbol{1}, Stack{StackSymbol{1}}, ReturnSymbol{1}};
    Word word10{CallSymbol{1}, Stack{StackSymbol{2}}, ReturnSymbol{1}};
};

TEST_F(TestVPA, default1)
{
    EXPECT_EQ(sut->checkWord(word1), true);
}
TEST_F(TestVPA, default2)
{
    EXPECT_EQ(sut->checkWord(word2), true);
}
TEST_F(TestVPA, default3)
{
    EXPECT_EQ(sut->checkWord(word3), true);
}

TEST_F(TestVPA, default4)
{
    EXPECT_EQ(sut->checkWord(word4), false);
}
TEST_F(TestVPA, default5)
{
    EXPECT_EQ(sut->checkWord(word5), false);
}

TEST_F(TestVPA, default6)
{
    EXPECT_EQ(sut->checkWord(word6), false);
}

TEST_F(TestVPA, default7)
{
    EXPECT_EQ(sut->checkWord(word7), false);
}
TEST_F(TestVPA, default8)
{
    EXPECT_EQ(sut->checkWord(word8), false);
}

TEST_F(TestVPA, default9)
{
    EXPECT_EQ(sut->checkWord(word9), true);
}
TEST_F(TestVPA, default10)
{
    EXPECT_EQ(sut->checkWord(word10), false);
}

} // namespace common::transition
