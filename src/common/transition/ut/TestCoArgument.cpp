#include <gtest/gtest.h>

#include "common/transition/CoArgument.hpp"

namespace common::transition
{
TEST(CoArgument, defaultTest)
{
    State s{15};
    CoArgument sut(s, symbol::StackSymbol(42));
    EXPECT_EQ(sut.state, 15);
    EXPECT_EQ(sut.stackSymbol, 42);
}

} // namespace common::transition
