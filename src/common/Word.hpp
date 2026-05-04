#pragma once

#include <cstddef>
#include <iostream>
#include <variant>
#include <vector>

#include "common/Stack.hpp"
#include "common/symbol/Symbols.hpp"

namespace common
{
using ControlWord = Stack;
using Symbol =
    std::variant<symbol::CallSymbol, symbol::ReturnSymbol, symbol::LocalSymbol, ControlWord>;
using Word = std::vector<Symbol>;

inline constexpr std::size_t CallSymbolVariant = 0;
inline constexpr std::size_t ReturnSymbolVariant = 1;
inline constexpr std::size_t LocalSymbolVariant = 2;
inline constexpr std::size_t ControlWordSymbolVariant = 3;

Word operator+(const Word &a, const Word &b);
Word &operator+=(Word &lhs, const Word &rhs);
Word &operator+=(Word &lhs, const Symbol &rhs);
std::ostream &operator<<(std::ostream &os, const Symbol &symbol);
std::ostream &operator<<(std::ostream &os, const Word &word);

struct WordHasher
{
    size_t operator()(const Word &word) const
    {
        size_t seed = word.size();

        for (const auto &symbol : word)
        {
            size_t symbolHash = symbol.index();

            switch (symbol.index())
            {
            case CallSymbolVariant:
                hash_combine(
                    symbolHash, static_cast<uint16_t>((std::get<symbol::CallSymbol>(symbol))));
                break;
            case ReturnSymbolVariant:
                hash_combine(
                    symbolHash, static_cast<uint16_t>((std::get<symbol::ReturnSymbol>(symbol))));
                break;
            case LocalSymbolVariant:
                hash_combine(
                    symbolHash, static_cast<uint16_t>((std::get<symbol::LocalSymbol>(symbol))));
                break;
            case ControlWordSymbolVariant:
                break;
            }

            hash_combine(seed, symbolHash);
        }

        return seed;
    }

private:
    inline void hash_combine(size_t &seed, uint16_t value) const
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};
} // namespace common