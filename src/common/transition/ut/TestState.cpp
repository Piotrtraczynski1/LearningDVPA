#include <gtest/gtest.h>

#include "../State.hpp"

namespace common::transition
{
TEST(State, defaultTest)
{
    State s{13, true};
    EXPECT_EQ(s.isAccepted, true);
    EXPECT_EQ(s.identifier, 13);
}

} // namespace common::transition
