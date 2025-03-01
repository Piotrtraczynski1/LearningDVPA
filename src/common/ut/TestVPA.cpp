#include <gtest/gtest.h>
#include <map>
#include <stdexcept>

#include "../VPA.hpp"
#include "../transition/Transition.cpp"

using namespace common::symbol;

namespace common::transition
{
class TestVPA : public ::testing::Test
{
    using Symbol = std::variant<symbol::CallSymbol, symbol::ReturnSymbol, symbol::LocalSymbol>;

public:
    void SetUp() override
    {
        callT[Argument<CallSymbol>{CallSymbol{1}, StackSymbol::BOTTOM, initialState}] =
            CoArgument{StackSymbol{1}, secondState};
        callT[Argument<CallSymbol>{CallSymbol{1}, StackSymbol{1}, secondState}] =
            CoArgument{StackSymbol{2}, secondState};
        callT[Argument<CallSymbol>{CallSymbol{1}, StackSymbol{2}, secondState}] =
            CoArgument{StackSymbol{2}, secondState};

        localT[Argument<LocalSymbol>{LocalSymbol{1}, StackSymbol::BOTTOM, initialState}] =
            initialState;
        localT[Argument<LocalSymbol>{LocalSymbol{1}, StackSymbol{1}, secondState}] = secondState;
        localT[Argument<LocalSymbol>{LocalSymbol{1}, StackSymbol{2}, secondState}] = secondState;

        localT[Argument<LocalSymbol>{LocalSymbol{2}, StackSymbol::BOTTOM, initialState}] =
            initialState;
        localT[Argument<LocalSymbol>{LocalSymbol{2}, StackSymbol{1}, secondState}] = secondState;
        localT[Argument<LocalSymbol>{LocalSymbol{2}, StackSymbol{2}, secondState}] = secondState;

        returnT[Argument<ReturnSymbol>{ReturnSymbol{1}, StackSymbol{1}, secondState}] =
            initialState;
        returnT[Argument<ReturnSymbol>{ReturnSymbol{1}, StackSymbol{2}, secondState}] = secondState;

        transition = new Transition{callT, returnT, localT};
        sut = new VPA{*transition, initialState};
    }

    State initialState{1, true};
    State secondState{2, false};

    std::map<Argument<symbol::CallSymbol>, CoArgument> callT;
    std::map<Argument<symbol::ReturnSymbol>, State> returnT;
    std::map<Argument<symbol::LocalSymbol>, State> localT;

    VPA *sut;
    Transition *transition;

    // poprawne nawiasowania:
    std::vector<Symbol> word1{CallSymbol{1}, ReturnSymbol{1}};
    std::vector<Symbol> word2{CallSymbol{1},   CallSymbol{1},   CallSymbol{1}, ReturnSymbol{1},
                              ReturnSymbol{1}, ReturnSymbol{1}, CallSymbol{1}, ReturnSymbol{1},
                              CallSymbol{1},   ReturnSymbol{1}, CallSymbol{1}, ReturnSymbol{1}};

    std::vector<Symbol> word3{LocalSymbol{1},  LocalSymbol{2}, CallSymbol{1}, LocalSymbol{1},
                              ReturnSymbol{1}, LocalSymbol{2}, LocalSymbol{1}};

    // niepoprawne nawiasowania:
    std::vector<Symbol> word4{LocalSymbol{1}, LocalSymbol{2}, CallSymbol{1}};
    std::vector<Symbol> word5{LocalSymbol{1}, LocalSymbol{2},  CallSymbol{1},  CallSymbol{1},
                              CallSymbol{1},  ReturnSymbol{1}, ReturnSymbol{1}};

    // Wyjatek
    std::vector<Symbol> word6{CallSymbol{2}};
    std::vector<Symbol> word7{LocalSymbol{1},  LocalSymbol{2}, CallSymbol{1},
                              ReturnSymbol{1}, LocalSymbol{2}, ReturnSymbol{1}};
    std::vector<Symbol> word8{LocalSymbol{1},  LocalSymbol{2}, CallSymbol{1}, LocalSymbol{1},
                              ReturnSymbol{1}, LocalSymbol{2}, LocalSymbol{3}};
};

TEST_F(TestVPA, default1) { EXPECT_EQ(sut->checkWord(word1), true); }
TEST_F(TestVPA, default2) { EXPECT_EQ(sut->checkWord(word2), true); }
TEST_F(TestVPA, default3) { EXPECT_EQ(sut->checkWord(word3), true); }

TEST_F(TestVPA, default4) { EXPECT_EQ(sut->checkWord(word4), false); }
TEST_F(TestVPA, default5) { EXPECT_EQ(sut->checkWord(word5), false); }

TEST_F(TestVPA, default6) { EXPECT_THROW(sut->checkWord(word6), std::out_of_range); }
TEST_F(TestVPA, default7) { EXPECT_THROW(sut->checkWord(word7), std::out_of_range); }
TEST_F(TestVPA, default8) { EXPECT_THROW(sut->checkWord(word8), std::out_of_range); }
} // namespace common::transition
