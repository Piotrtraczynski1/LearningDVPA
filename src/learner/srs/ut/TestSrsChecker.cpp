#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "learner/srs/SrsChecker.hpp"

using namespace testing;
using namespace common::symbol;
using common::transition::State;

namespace learner::srs
{
namespace
{
constexpr uint16_t numOfCalls{2};
constexpr uint16_t numOfReturns{2};
constexpr uint16_t numOfLocals{3};
constexpr uint16_t numOfStackSymbols{3};
constexpr uint16_t numOfStates{6};
constexpr transition::State initialState{0};
} // namespace

struct TestSrsCheckerParams
{
    Srs srs{};
    std::vector<uint16_t> acceptingStates{};
    bool rejectingEmpty{false};
    bool acceptingEmpty{false};
};

class TestSrsChecker : public TestWithParam<TestSrsCheckerParams>
{
public:
    void SetUp() override
    {
        initTransition();
        initSrs();
        initAcceptingStates();
        initOracle();

        hypothesis = std::make_shared<common::VPA<AutomatonKind::Normal>>(
            std::move(transition), initialState, acceptingStates, numOfStates);

        sut = std::make_shared<SrsChecker>(
            srs, *oracle, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
    }

    virtual void initOracle() = 0;

    void initTransition()
    {
        transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();
        // Locals:
        transition->add(State{0}, LocalSymbol{0}, State{1});
        transition->add(State{1}, LocalSymbol{0}, State{2});
        transition->add(State{2}, LocalSymbol{0}, State{3});
        transition->add(State{3}, LocalSymbol{0}, State{4});
        transition->add(State{4}, LocalSymbol{0}, State{5});
        transition->add(State{5}, LocalSymbol{0}, State{0});

        transition->add(State{0}, LocalSymbol{1}, State{4});
        transition->add(State{1}, LocalSymbol{1}, State{3});
        transition->add(State{2}, LocalSymbol{1}, State{5});
        transition->add(State{3}, LocalSymbol{1}, State{3});
        transition->add(State{4}, LocalSymbol{1}, State{2});
        transition->add(State{5}, LocalSymbol{1}, State{5});

        transition->add(State{0}, LocalSymbol{2}, State{5});
        transition->add(State{1}, LocalSymbol{2}, State{2});
        transition->add(State{2}, LocalSymbol{2}, State{1});
        transition->add(State{3}, LocalSymbol{2}, State{4});
        transition->add(State{4}, LocalSymbol{2}, State{3});
        transition->add(State{5}, LocalSymbol{2}, State{3});

        // Calls:
        transition->add(State{0}, CallSymbol{0}, State{3}, StackSymbol{1});
        transition->add(State{1}, CallSymbol{0}, State{5}, StackSymbol{1});
        transition->add(State{2}, CallSymbol{0}, State{4}, StackSymbol{1});
        transition->add(State{3}, CallSymbol{0}, State{2}, StackSymbol{1});
        transition->add(State{4}, CallSymbol{0}, State{0}, StackSymbol{1});
        transition->add(State{5}, CallSymbol{0}, State{0}, StackSymbol{1});

        transition->add(State{0}, CallSymbol{1}, State{1}, StackSymbol{1});
        transition->add(State{1}, CallSymbol{1}, State{1}, StackSymbol{1});
        transition->add(State{2}, CallSymbol{1}, State{2}, StackSymbol{1});
        transition->add(State{3}, CallSymbol{1}, State{2}, StackSymbol{1});
        transition->add(State{4}, CallSymbol{1}, State{5}, StackSymbol{1});
        transition->add(State{5}, CallSymbol{1}, State{3}, StackSymbol{1});

        // Returns:
        transition->add(State{0}, StackSymbol{0}, ReturnSymbol{0}, State{5});
        transition->add(State{1}, StackSymbol{0}, ReturnSymbol{0}, State{4});
        transition->add(State{2}, StackSymbol{0}, ReturnSymbol{0}, State{3});
        transition->add(State{3}, StackSymbol{0}, ReturnSymbol{0}, State{1});
        transition->add(State{4}, StackSymbol{0}, ReturnSymbol{0}, State{2});
        transition->add(State{5}, StackSymbol{0}, ReturnSymbol{0}, State{0});

        transition->add(State{0}, StackSymbol{1}, ReturnSymbol{0}, State{2});
        transition->add(State{1}, StackSymbol{1}, ReturnSymbol{0}, State{2});
        transition->add(State{2}, StackSymbol{1}, ReturnSymbol{0}, State{3});
        transition->add(State{3}, StackSymbol{1}, ReturnSymbol{0}, State{1});
        transition->add(State{4}, StackSymbol{1}, ReturnSymbol{0}, State{4});
        transition->add(State{5}, StackSymbol{1}, ReturnSymbol{0}, State{5});

        transition->add(State{0}, StackSymbol{2}, ReturnSymbol{0}, State{1});
        transition->add(State{1}, StackSymbol{2}, ReturnSymbol{0}, State{2});
        transition->add(State{2}, StackSymbol{2}, ReturnSymbol{0}, State{4});
        transition->add(State{3}, StackSymbol{2}, ReturnSymbol{0}, State{3});
        transition->add(State{4}, StackSymbol{2}, ReturnSymbol{0}, State{2});
        transition->add(State{5}, StackSymbol{2}, ReturnSymbol{0}, State{5});

        transition->add(State{0}, StackSymbol{0}, ReturnSymbol{1}, State{5});
        transition->add(State{1}, StackSymbol{0}, ReturnSymbol{1}, State{5});
        transition->add(State{2}, StackSymbol{0}, ReturnSymbol{1}, State{3});
        transition->add(State{3}, StackSymbol{0}, ReturnSymbol{1}, State{2});
        transition->add(State{4}, StackSymbol{0}, ReturnSymbol{1}, State{2});
        transition->add(State{5}, StackSymbol{0}, ReturnSymbol{1}, State{1});

        transition->add(State{0}, StackSymbol{1}, ReturnSymbol{1}, State{3});
        transition->add(State{1}, StackSymbol{1}, ReturnSymbol{1}, State{3});
        transition->add(State{2}, StackSymbol{1}, ReturnSymbol{1}, State{2});
        transition->add(State{3}, StackSymbol{1}, ReturnSymbol{1}, State{1});
        transition->add(State{4}, StackSymbol{1}, ReturnSymbol{1}, State{1});
        transition->add(State{5}, StackSymbol{1}, ReturnSymbol{1}, State{2});

        transition->add(State{0}, StackSymbol{2}, ReturnSymbol{1}, State{5});
        transition->add(State{1}, StackSymbol{2}, ReturnSymbol{1}, State{5});
        transition->add(State{2}, StackSymbol{2}, ReturnSymbol{1}, State{3});
        transition->add(State{3}, StackSymbol{2}, ReturnSymbol{1}, State{2});
        transition->add(State{4}, StackSymbol{2}, ReturnSymbol{1}, State{1});
        transition->add(State{5}, StackSymbol{2}, ReturnSymbol{1}, State{1});
    }

    void initSrs()
    {
        srs = GetParam().srs;
    }

    void initAcceptingStates()
    {
        acceptingStates = GetParam().acceptingStates;
    }

    bool isValidSymbol(const common::Symbol &symbol)
    {
        const common::Symbol c0{CallSymbol{0}};
        const common::Symbol c1{CallSymbol{1}};

        const common::Symbol r0{ReturnSymbol{0}};
        const common::Symbol r1{ReturnSymbol{1}};

        const common::Symbol l0{LocalSymbol{0}};
        const common::Symbol l1{LocalSymbol{1}};
        const common::Symbol l2{LocalSymbol{2}};

        if (symbol == c0 or symbol == c1 or symbol == r0 or symbol == r1 or symbol == l0 or
            symbol == l1 or symbol == l2)
        {
            return true;
        }
        return false;
    }

    virtual void checkOutput(const common::Word &output)
    {
        for (const auto &symbol : output)
        {
            ASSERT_TRUE(isValidSymbol(symbol));
        }
    }

    std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> transition;
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis;
    std::shared_ptr<teacher::Converter> converter;
    std::shared_ptr<teacher::Teacher> oracle;
    std::vector<uint16_t> acceptingStates{};

    std::shared_ptr<SrsChecker> sut;
    Srs srs{};
};

class TestSrsCheckerRejectingOracle : public TestSrsChecker
{
public:
    void initOracle() override
    {
        rejectingTransition =
            std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

        vpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
            std::move(rejectingTransition), initialState, std::vector<uint16_t>{}, 1);

        converter = std::make_shared<teacher::Converter>(
            vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

        oracle = std::make_shared<teacher::Teacher>(vpa, converter);
    }

    void checkOutput(common::Word output)
    {
        if (GetParam().rejectingEmpty)
        {
            EXPECT_EQ(output, common::Word{});
            return;
        }
        TestSrsChecker::checkOutput(output);
        EXPECT_TRUE(hypothesis->checkWord(output));
        return;
    }

    std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> rejectingTransition;
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa;
};

TEST_P(TestSrsCheckerRejectingOracle, DefaultTest)
{
    checkOutput(sut->check(hypothesis));
}

class TestSrsCheckerAcceptingOracle : public TestSrsChecker
{
public:
    void initOracle() override
    {
        acceptingTransition =
            std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

        acceptingTransition->add(State{0}, LocalSymbol{0}, State{0});
        acceptingTransition->add(State{0}, LocalSymbol{1}, State{0});
        acceptingTransition->add(State{0}, LocalSymbol{2}, State{0});

        acceptingTransition->add(State{0}, CallSymbol{0}, State{0}, StackSymbol{1});
        acceptingTransition->add(State{0}, CallSymbol{1}, State{0}, StackSymbol{1});

        acceptingTransition->add(State{0}, StackSymbol{0}, ReturnSymbol{0}, State{0});
        acceptingTransition->add(State{0}, StackSymbol{1}, ReturnSymbol{0}, State{0});
        acceptingTransition->add(State{0}, StackSymbol{0}, ReturnSymbol{1}, State{0});
        acceptingTransition->add(State{0}, StackSymbol{1}, ReturnSymbol{1}, State{0});

        vpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
            std::move(acceptingTransition), initialState, std::vector<uint16_t>{0}, 1);

        converter = std::make_shared<teacher::Converter>(
            vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);

        oracle = std::make_shared<teacher::Teacher>(vpa, converter);
    }

    void checkOutput(common::Word output)
    {
        if (GetParam().acceptingEmpty)
        {
            EXPECT_EQ(output, common::Word{});
            return;
        }
        TestSrsChecker::checkOutput(output);
        EXPECT_FALSE(hypothesis->checkWord(output));
        return;
    }

    std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> acceptingTransition;
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa;
};

TEST_P(TestSrsCheckerAcceptingOracle, DefaultTest)
{
    checkOutput(sut->check(hypothesis));
}

namespace
{
using W = common::Word;
using C = CallSymbol;
using R = ReturnSymbol;
using L = LocalSymbol;
using Accepting = std::vector<uint16_t>;

// clang-format off
const std::vector<TestSrsCheckerParams> testSrsCheckerParams{
    TestSrsCheckerParams{
        .srs = Srs{
            SrsRuleWithParams{.right = {.left = W{L{0}}, .right = W{L{1}}}}
        },
        .acceptingStates = {1, 2}
    }, 
    TestSrsCheckerParams{
        .srs = Srs{
            SrsRuleWithParams{.right = {.left = W{C{0}}, .right = W{R{0}}}}
        },
        .acceptingStates = {0, 4}
    },
    TestSrsCheckerParams{
        .srs = Srs{
            SrsRuleWithParams{.right = {.left = W{C{0}, C{0}, C{0}, C{0}, C{0}}, .right = W{R{0}, R{0}, R{0}, R{0}, R{0}}}}
        },
        .acceptingStates = {0, 1, 2, 3, 4}
    },
    TestSrsCheckerParams{
        .srs = Srs{
            SrsRuleWithParams{.right = {.left = W{C{0}, C{0}, C{0}, C{0}, C{0}}, .right = W{R{0}, R{0}, R{0}, R{0}, R{0}}}}
        },
        .acceptingStates = {5}
    },
    TestSrsCheckerParams{
        .srs = Srs{
            SrsRuleWithParams{.right = {.left = W{L{1}}, .right = W{L{2}}}}
        },
        .acceptingStates = {0, 1, 2, 3, 4, 5},
        .acceptingEmpty = true
    },
    TestSrsCheckerParams{
        .srs = Srs{
            SrsRuleWithParams{.right = {.left = W{L{1}}, .right = W{L{2}}}}
        },
        .acceptingStates = {},
        .rejectingEmpty = true
    },
    TestSrsCheckerParams{
        .srs = Srs{
            SrsRuleWithParams{.right = {.left = W{}, .right = W{}}}
        },
        .acceptingStates = {1, 3, 4},
        .rejectingEmpty = true,
        .acceptingEmpty = true
    }
    };
// clang-format on

} // namespace

INSTANTIATE_TEST_SUITE_P(
    DefaultTestRejecting, TestSrsCheckerRejectingOracle, ::testing::ValuesIn(testSrsCheckerParams));

INSTANTIATE_TEST_SUITE_P(
    DefaultTestAccepting, TestSrsCheckerAcceptingOracle, ::testing::ValuesIn(testSrsCheckerParams));

} // namespace learner::srs
