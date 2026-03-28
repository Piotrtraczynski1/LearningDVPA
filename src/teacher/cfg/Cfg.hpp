#pragma once

#include <limits>
#include <memory>
#include <optional>
#include <vector>

#include "teacher/cfg/NonTerminal.hpp"
#include "teacher/cfg/Terminal.hpp"

namespace teacher::cfg
{

struct Witness
{
    Terminal terminal{Terminal::INVALID};
    uint32_t n1Index{std::numeric_limits<uint32_t>::max()};
    uint32_t n2Index{std::numeric_limits<uint32_t>::max()};
};

class Cfg
{
    using OutputType = std::optional<std::vector<Terminal>>;

    std::vector<bool> isGenerative{};
    std::vector<Witness> witnesses{};
    std::vector<uint32_t> generativeNonTerminalsQueue{};

    std::unique_ptr<uint32_t[]> nonTerminalIndexes;

    std::vector<uint8_t> unresolvedNonTerminalsPerProj{};
    std::vector<uint32_t> nonTerminalIndexPerProj{};
    std::vector<uint32_t> firstNonTerminalPerProj{};
    std::vector<uint32_t> secondNonTerminalPerProj{};
    std::vector<Terminal> terminalValuePerProj{};

    std::vector<uint32_t> numOfParentProductions{};
    std::vector<uint32_t> parentProductions{};
    std::vector<uint64_t> parentProductionsOffsets{};

    uint64_t numOfProductions{0};
    uint32_t numOfNonTerminals{1};
    static constexpr uint32_t unknownIndex{std::numeric_limits<uint32_t>::max()};

public:
    Cfg();
    Cfg(const size_t estimatedNumberOfProductions, const size_t estimatedNumberOfNonTerminals);

    void addProduction(
        const NonTerminal nonTerminal, const Terminal terminal, const NonTerminal n1,
        const NonTerminal n2);
    std::shared_ptr<OutputType> isEmpty();

private:
    void insertDummyElements();
    void calculateParentProductions();
    uint32_t setNonTerminalIndex(const NonTerminal nonTerminal);
    void buildExample(std::vector<Terminal> &example, const uint32_t id);
};
} // namespace teacher::cfg