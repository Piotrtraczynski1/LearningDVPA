#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

#include "teacher/cfg/NonTerminal.hpp"
#include "teacher/cfg/Projection.hpp"
#include "teacher/cfg/Terminal.hpp"

namespace teacher::cfg
{
class Cfg
{
    using OutputType = std::optional<std::vector<Terminal>>;

    std::vector<Projection> projections{};
    std::vector<uint8_t> unresolved{};
    std::vector<std::vector<uint64_t>> uses{};
    std::vector<bool> gen{};
    std::vector<uint64_t> witnesses{};
    std::queue<uint64_t> que{};
    std::unordered_map<NonTerminal, uint64_t> nonTerminalIndexes{};

    uint64_t numOfProjections{0};
    uint64_t numOfNonTerminals{0};

public:
    Cfg() = default;
    Cfg(const size_t estimatedNumberOfProjections, const size_t estimatedNumberOfNonTerminals);

    void addProjection(
        NonTerminal nonTerminal, std::optional<Terminal> terminal, std::optional<NonTerminal> n1,
        std::optional<NonTerminal> n2);
    std::shared_ptr<OutputType> isEmpty();

    void print(std::ostream &os = std::cout);

private:
    uint64_t setNonTerminalIndex(const NonTerminal nonTerminal);
    void buildExample(std::vector<Terminal> &example, const uint64_t id);
};
} // namespace teacher::cfg