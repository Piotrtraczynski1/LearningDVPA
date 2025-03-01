#include <gtest/gtest.h>

#include "../Argument.hpp"

using namespace common::symbol;

namespace common::transition
{
TEST(Argument, callArgument)
{
    State s{15, true};
    Argument<CallSymbol> sut(CallSymbol(13), StackSymbol(42), s);
    EXPECT_EQ(sut.state.isAccepted, true);
    EXPECT_EQ(sut.state.identifier, 15);
    EXPECT_EQ(sut.alphabetSymbol, 13);
    EXPECT_EQ(sut.stackSymbol, 42);
}

} // namespace common::transition
