#include <gtest/gtest.h>
#include <map>
#include <stdexcept>

#include "../Transition.cpp"

using namespace common::symbol;

namespace common::transition
{
class TestTransition : public ::testing::Test
{
public:
    void SetUp() override
    {
        callT[argCall] = coArgCall;
        returnT[argReturn] = returnState;
        localT[argLocaL] = localState;

        sut = Transition(callT, returnT, localT);
    }

    std::map<Argument<symbol::CallSymbol>, CoArgument> callT;
    std::map<Argument<symbol::ReturnSymbol>, State> returnT;
    std::map<Argument<symbol::LocalSymbol>, State> localT;

    State initialState{1, true};
    State callState{2, true};
    State returnState{3, false};
    State localState{4, true};

    Argument<CallSymbol> argCall{CallSymbol{1}, StackSymbol{1}, initialState};
    CoArgument coArgCall{StackSymbol{2}, callState};
    Argument<ReturnSymbol> argReturn{ReturnSymbol{1}, StackSymbol{1}, initialState};
    Argument<LocalSymbol> argLocaL{LocalSymbol{1}, StackSymbol{1}, initialState};

    Transition sut;
};

TEST_F(TestTransition, CallSymbolArgumentFound) { EXPECT_EQ(sut(argCall), coArgCall); }

TEST_F(TestTransition, CallSymbolArgumentNotFound)
{
    Argument<symbol::CallSymbol> invalidArg(CallSymbol{0}, StackSymbol{0}, State{0, false});
    EXPECT_THROW(sut(invalidArg), std::out_of_range);
}

TEST_F(TestTransition, ReturnSymbolArgumentFound) { EXPECT_EQ(sut(argReturn), returnState); }

TEST_F(TestTransition, ReturnSymbolArgumentNotFound)
{
    Argument<symbol::ReturnSymbol> invalidArg(ReturnSymbol{0}, StackSymbol{0}, State{0, false});
    EXPECT_THROW(sut(invalidArg), std::out_of_range);
}

TEST_F(TestTransition, LocalSymbolArgumentFound) { EXPECT_EQ(sut(argLocaL), localState); }

TEST_F(TestTransition, LocalSymbolArgumentNotFound)
{
    Argument<symbol::LocalSymbol> invalidArg(LocalSymbol{0}, StackSymbol{0}, State{0, false});
    EXPECT_THROW(sut(invalidArg), std::out_of_range);
}
} // namespace common::transition
