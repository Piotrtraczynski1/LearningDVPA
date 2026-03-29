#include <cstdint>
#include <memory>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"
#include "helper/ExampleRun.hpp"
#include "learner/Learner.hpp"
#include "learner/srs/Srs.hpp"
#include "teacher/Teacher.hpp"

namespace helper
{

namespace
{
std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> createTransition()
{
    using common::symbol::CallSymbol;
    using common::symbol::LocalSymbol;
    using common::symbol::ReturnSymbol;
    using common::symbol::StackSymbol;
    using common::transition::State;

    auto transition = std::make_unique<common::transition::Transition<AutomatonKind::Normal>>();

    // Call transitions
    transition->add(State{0}, CallSymbol{0}, State{1}, StackSymbol{1});
    transition->add(State{1}, CallSymbol{0}, State{2}, StackSymbol{1});
    transition->add(State{2}, CallSymbol{0}, State{3}, StackSymbol{1});
    transition->add(State{3}, CallSymbol{0}, State{0}, StackSymbol{1});

    // Return transitions
    transition->add(State{0}, StackSymbol{0}, ReturnSymbol{0}, State{2});
    transition->add(State{1}, StackSymbol{0}, ReturnSymbol{0}, State{3});
    transition->add(State{2}, StackSymbol{0}, ReturnSymbol{0}, State{1});
    transition->add(State{3}, StackSymbol{0}, ReturnSymbol{0}, State{1});

    transition->add(State{0}, StackSymbol{1}, ReturnSymbol{0}, State{1});
    transition->add(State{1}, StackSymbol{1}, ReturnSymbol{0}, State{2});
    transition->add(State{2}, StackSymbol{1}, ReturnSymbol{0}, State{3});
    transition->add(State{3}, StackSymbol{1}, ReturnSymbol{0}, State{1});

    // Local transitions
    transition->add(State{0}, LocalSymbol{0}, State{3});
    transition->add(State{1}, LocalSymbol{0}, State{3});
    transition->add(State{2}, LocalSymbol{0}, State{1});
    transition->add(State{3}, LocalSymbol{0}, State{2});

    transition->add(State{0}, LocalSymbol{1}, State{2});
    transition->add(State{1}, LocalSymbol{1}, State{2});
    transition->add(State{2}, LocalSymbol{1}, State{1});
    transition->add(State{3}, LocalSymbol{1}, State{2});

    return std::move(transition);
}

std::vector<uint16_t> createAcceptingStates()
{
    return std::vector<uint16_t>{0, 2};
}

learner::srs::Srs createSrs()
{
    return {};
}

} // namespace

void runExample()
{
    constexpr uint16_t numOfStates{4};
    constexpr uint16_t numOfCalls{1};
    constexpr uint16_t numOfReturns{1};
    constexpr uint16_t numOfLocals{2};
    constexpr uint16_t numOfStackSymbols{2};
    constexpr common::transition::State initialState{0};

    std::unique_ptr<common::transition::Transition<AutomatonKind::Normal>> transition =
        createTransition();
    auto acceptingStates = createAcceptingStates();
    learner::srs::Srs srs = createSrs();

    auto vpa = std::make_shared<common::VPA<AutomatonKind::Normal>>(
        std::move(transition), initialState, acceptingStates, numOfStates);

    auto converter = std::make_shared<teacher::Converter>(
        vpa, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols);
    auto teacher = std::make_shared<teacher::Teacher>(vpa, converter);

    auto learner = std::make_shared<learner::Learner>(
        *teacher, numOfCalls, numOfReturns, numOfLocals, numOfStackSymbols, srs);

    learner->run()->print();
}
} // namespace helper