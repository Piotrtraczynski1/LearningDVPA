#include "Transition.hpp"

namespace common::transition
{
CoArgument Transition::operator()(Argument<symbol::CallSymbol> arg) { return callT.at(arg); }

State Transition::operator()(Argument<symbol::LocalSymbol> arg) { return localT.at(arg); }

State Transition::operator()(Argument<symbol::ReturnSymbol> arg) { return returnT.at(arg); }
} // namespace common::transition
