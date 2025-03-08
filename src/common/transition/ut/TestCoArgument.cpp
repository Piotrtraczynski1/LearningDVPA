#include <gtest/gtest.h>

#include "common/transition/CoArgument.hpp"

namespace common::transition
{
TEST(CoArgument, defaultTest)
{
    State s{15, true};
    CoArgument sut(symbol::StackSymbol(42), s);
    EXPECT_EQ(sut.state.isAccepted, true);
    EXPECT_EQ(sut.state.identifier, 15);
    EXPECT_EQ(sut.stackSymbol, 42);
}

} // namespace common::transition
