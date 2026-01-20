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

    std::vector<uint32_t> numOfParentProjections{};
    std::vector<uint32_t> parentProjections{};
    std::vector<uint64_t> parentProjectionsOffsets{};

    uint64_t numOfProjections{0};
    uint32_t numOfNonTerminals{1};
    static constexpr uint32_t unknownIndex{std::numeric_limits<uint32_t>::max()};

public:
    Cfg();
    Cfg(const size_t estimatedNumberOfProjections, const size_t estimatedNumberOfNonTerminals);

    void addProjection(
        const NonTerminal nonTerminal, const Terminal terminal, const NonTerminal n1,
        const NonTerminal n2);
    std::shared_ptr<OutputType> isEmpty();

private:
    void insertDummyElements();
    void calculateParentProjections();
    uint32_t setNonTerminalIndex(const NonTerminal nonTerminal);
    void buildExample(std::vector<Terminal> &example, const uint32_t id);
};
} // namespace teacher::cfg