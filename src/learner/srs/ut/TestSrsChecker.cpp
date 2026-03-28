#include <gtest/gtest.h>

#include "common/Stack.cpp"
#include "common/VPA.cpp"
#include "common/Word.cpp"
#include "common/transition/Transition.cpp"
#include "learner/srs/SrsChecker.cpp"
#include "teacher/Converter.cpp"
#include "teacher/Teacher.cpp"
#include "teacher/cfg/Calculator.cpp"
#include "teacher/cfg/Cfg.cpp"
#include "utils/TimeMarker.cpp"

using namespace testing;

namespace learner::srs
{
class TestSrsChecker : public Test
{
public:
    void SetUp() override
    {
    }

    // SrsChecker sut;
};

TEST_F(TestSrsChecker, default1)
{
    EXPECT_TRUE(true);
}

} // namespace learner::srs
