#include <functional>
#include <map>
#include <memory>

#include "benchmark/scenario/Scenario.hpp"
// clang-format off

// increase number of states with varying number of symbols
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesBase.hpp"

#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesFourCalls.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesSixCalls.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesEightCalls.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesTenCalls.hpp"

#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesFourReturns.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesSixReturns.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesEightReturns.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesTenReturns.hpp"

#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesFourLocals.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesSixLocals.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesEightLocals.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesTenLocals.hpp"



namespace benchmark
{
using ScenarioFactory = std::function<std::unique_ptr<scenario::Scenario>()>;


static const std::map<std::string, ScenarioFactory> scenariosRegistry{
    {"increasing-number-of-states", [] { return std::make_unique<scenario::IncreasingNumOfStatesBase>(); }},
    {"increasing-number-of-states-four-calls", [] { return std::make_unique<scenario::IncreasingNumOfStatesFourCalls>(); }},
    {"increasing-number-of-states-six-calls", [] { return std::make_unique<scenario::IncreasingNumOfStatesSixCalls>(); }},
    {"increasing-number-of-states-eight-calls", [] { return std::make_unique<scenario::IncreasingNumOfStatesEightCalls>(); }},
    {"increasing-number-of-states-ten-calls", [] { return std::make_unique<scenario::IncreasingNumOfStatesTenCalls>(); }},
    {"increasing-number-of-states-four-returns", [] { return std::make_unique<scenario::IncreasingNumOfStatesFourReturns>(); }},
    {"increasing-number-of-states-six-returns", [] { return std::make_unique<scenario::IncreasingNumOfStatesSixReturns>(); }},
    {"increasing-number-of-states-eight-returns", [] { return std::make_unique<scenario::IncreasingNumOfStatesEightReturns>(); }},
    {"increasing-number-of-states-ten-returns", [] { return std::make_unique<scenario::IncreasingNumOfStatesTenReturns>(); }},
    {"increasing-number-of-states-four-locals", [] { return std::make_unique<scenario::IncreasingNumOfStatesFourLocals>(); }},
    {"increasing-number-of-states-six-locals", [] { return std::make_unique<scenario::IncreasingNumOfStatesSixLocals>(); }},
    {"increasing-number-of-states-eight-locals", [] { return std::make_unique<scenario::IncreasingNumOfStatesEightLocals>(); }},
    {"increasing-number-of-states-ten-locals", [] { return std::make_unique<scenario::IncreasingNumOfStatesTenLocals>(); }}
};
// clang-format on

} // namespace benchmark