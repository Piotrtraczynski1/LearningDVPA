#include <gtest/gtest.h>
#include <map>
#include <stack>

#include "common/Stack.cpp"
#include "common/Word.cpp"
#include "common/transition/Transition.cpp"

using namespace common::symbol;

namespace common
{
class TestStack : public ::testing::Test
{
public:
    void SetUp() override
    {
        s1.push(StackSymbol{2});
        s1.push({StackSymbol{1}});

        s2.push(StackSymbol{2});
        s2.push({StackSymbol{2}});
    }

    Stack s1;
    Stack s2;
};

TEST_F(TestStack, default1)
{
    EXPECT_EQ(s1 != s2, true);
}

} // namespace common
