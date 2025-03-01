#pragma once

#include <cstdint>

namespace common::transition
{
struct State
{
public:
    uint16_t identifier;
    bool isAccepted;

    State() = default; // Remove this constructor !!
    State(uint16_t id, bool isAccept) : identifier{id}, isAccepted{isAccept} {}

    bool operator==(const State &state) const { return identifier == state.identifier; }
    bool operator<(const State &state) const { return identifier < state.identifier; }
};
} // namespace common::transition
