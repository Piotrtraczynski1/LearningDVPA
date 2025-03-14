#include <gtest/gtest.h>
#include <map>
#include <stdexcept>

#include "common/transition/Transition.cpp"

using namespace common::symbol;

namespace common::transition
{
class TestTransition : public ::testing::Test
{
public:
    void SetUp() override
    {
        sut = std::make_unique<Transition>();
        sut->add(initialState, callSymbol, callState, StackSymbol{2});
        sut->add(initialState, StackSymbol{1}, returnSymbol, returnState);
        sut->add(initialState, localSymbol, localState);
    }

    State initialState{1};
    State callState{2};
    State returnState{3};
    State localState{4};

    CallSymbol callSymbol{1};
    ReturnSymbol returnSymbol{1};
    LocalSymbol localSymbol{1};

    CoArgument coArgCall{callState, StackSymbol{2}};

    std::unique_ptr<Transition> sut;
};

TEST_F(TestTransition, CallSymbolArgumentFound)
{
    EXPECT_EQ((*sut)(initialState, callSymbol), coArgCall);
}

TEST_F(TestTransition, CallSymbolArgumentNotFound)
{
    CoArgument invalid{State::INVALID, StackSymbol::INVALID};
    EXPECT_EQ((*sut)(State{0}, CallSymbol{0}), invalid);
}

TEST_F(TestTransition, ReturnSymbolArgumentFound)
{
    EXPECT_EQ((*sut)(initialState, StackSymbol{1}, returnSymbol), returnState);
}

TEST_F(TestTransition, ReturnSymbolArgumentNotFound)
{
    EXPECT_EQ((*sut)(initialState, StackSymbol{5}, ReturnSymbol{0}), State::INVALID);
}

TEST_F(TestTransition, LocalSymbolArgumentFound)
{
    EXPECT_EQ((*sut)(initialState, localSymbol), localState);
}

TEST_F(TestTransition, LocalSymbolArgumentNotFound)
{
    EXPECT_EQ((*sut)(initialState, LocalSymbol{5}), State::INVALID);
}
} // namespace common::transition
