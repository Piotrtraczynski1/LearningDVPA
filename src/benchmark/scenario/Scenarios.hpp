#include <functional>
#include <map>
#include <memory>

#include "benchmark/scenario/Scenario.hpp"
// clang-format off

// increase number of states with varying number of symbols
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesBase.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesAndCalls.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesAndReturns.hpp"
#include "benchmark/scenario/increasingNumOfStates/IncreasingNumOfStatesAndLocals.hpp"

// srs scenarios
#include "benchmark/scenario/srs/CancellationIncreasingNumOfStates.hpp"
#include "benchmark/scenario/srs/CommutationIncreasingNumOfStates.hpp"
#include "benchmark/scenario/srs/IdempotencyIncreasingNumOfStates.hpp"

namespace benchmark
{
using ScenarioFactory = std::function<std::unique_ptr<scenario::Scenario>()>;

static const std::map<std::string, ScenarioFactory> scenariosRegistry{
    {"increasing-number-of-states-base", [] { return std::make_unique<scenario::IncreasingNumOfStatesBase>(); }},
    {"increasing-number-of-states-and-calls", [] { return std::make_unique<scenario::IncreasingNumOfStatesAndCalls>(); }},
    {"increasing-number-of-states-and-returns", [] { return std::make_unique<scenario::IncreasingNumOfStatesAndReturns>(); }},
    {"increasing-number-of-states-and-locals", [] { return std::make_unique<scenario::IncreasingNumOfStatesAndLocals>(); }},
    {"cancellation-increasing-number-of-states", [] { return std::make_unique<scenario::CancellationIncreasingNumOfStates>(); }},
    {"commutation-increasing-number-of-states", [] { return std::make_unique<scenario::CommutationIncreasingNumOfStates>(); }},
    {"idempotency-increasing-number-of-states", [] { return std::make_unique<scenario::IdempotencyIncreasingNumOfStates>(); }}
};
// clang-format on

} // namespace benchmark