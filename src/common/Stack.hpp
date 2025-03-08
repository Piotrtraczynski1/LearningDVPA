#pragma once

#include <stack>

#include "common/symbol/Symbols.hpp"

namespace common
{
class Stack
{
    std::stack<symbol::StackSymbol> stack;

public:
    Stack() = default;
    Stack(std::initializer_list<symbol::StackSymbol> initList);
    bool operator!=(const Stack &s) const { return stack != s.stack; };

    symbol::StackSymbol top() const;
    void pop();
    void push(symbol::StackSymbol symbol);
    size_t size() const;
};
} // namespace common