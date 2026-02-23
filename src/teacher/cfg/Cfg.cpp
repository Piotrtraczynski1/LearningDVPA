#include "teacher/cfg/Cfg.hpp"
#include "common/Word.hpp"
#include "utils/Constants.hpp"
#include "utils/ExitCode.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace teacher::cfg
{
Cfg::Cfg()
{
    nonTerminalIndexes.reset(new uint32_t[utils::MaxNumOfNonTerminals]());
    insertDummyElements();
}

Cfg::Cfg(const size_t estimatedNumberOfProjections, const size_t estimatedNumberOfNonTerminals)
{
    unresolvedNonTerminalsPerProj.reserve(estimatedNumberOfProjections);
    nonTerminalIndexPerProj.reserve(estimatedNumberOfProjections);
    firstNonTerminalPerProj.reserve(estimatedNumberOfProjections);
    secondNonTerminalPerProj.reserve(estimatedNumberOfProjections);
    terminalValuePerProj.reserve(estimatedNumberOfProjections);

    nonTerminalIndexes.reset(new uint32_t[utils::MaxNumOfNonTerminals]());
    generativeNonTerminalsQueue.reserve(estimatedNumberOfNonTerminals);
    isGenerative.reserve(estimatedNumberOfNonTerminals);
    witnesses.reserve(estimatedNumberOfNonTerminals);
    numOfParentProjections.reserve(estimatedNumberOfNonTerminals);

    insertDummyElements();
}

void Cfg::insertDummyElements()
{
    isGenerative.push_back(false);
    witnesses.push_back(Witness{});
    numOfParentProjections.push_back(0);
}

void Cfg::addProjection(
    const NonTerminal nonTerminal, const Terminal terminal, const NonTerminal n1,
    const NonTerminal n2)
{
    const auto nonTerminalId = setNonTerminalIndex(nonTerminal);
    nonTerminalIndexPerProj.push_back(nonTerminalId);

    uint8_t deps{0};
    uint32_t n1Index{unknownIndex};
    if (n1 != NonTerminal::INVALID)
    {
        n1Index = setNonTerminalIndex(n1);
        numOfParentProjections[n1Index]++;
        deps++;
    }
    uint32_t n2Index{unknownIndex};
    if (n2 != NonTerminal::INVALID)
    {
        n2Index = setNonTerminalIndex(n2);
        numOfParentProjections[n2Index]++;
        deps++;
    }

    firstNonTerminalPerProj.push_back(n1Index);
    secondNonTerminalPerProj.push_back(n2Index);
    terminalValuePerProj.push_back(terminal);
    unresolvedNonTerminalsPerProj.push_back(deps);

    if (deps == 0 and not isGenerative[nonTerminalId])
    {
        isGenerative[nonTerminalId] = true;
        witnesses[nonTerminalId] = {.terminal = terminal, .n1Index = n1Index, .n2Index = n2Index};
        generativeNonTerminalsQueue.push_back(nonTerminalId);
    }

    numOfProjections++;

    if (numOfProjections == unknownIndex)
    {
        ERR("[CFG]: Too many projections!");
        exit(toExit(ExitCode::UNDEFINED));
    }
}

uint32_t Cfg::setNonTerminalIndex(const NonTerminal nonTerminal)
{
    uint32_t &nonTerminalIndex = nonTerminalIndexes[static_cast<uint32_t>(nonTerminal)];

    if (nonTerminalIndex != 0)
    {
        return nonTerminalIndex;
    }

    nonTerminalIndex = numOfNonTerminals++;
    insertDummyElements();

    return nonTerminalIndex;
}

void Cfg::calculateParentProjections()
{
    TIME_MARKER("[CFG]: calculateParentProjections");

    parentProjectionsOffsets.resize(numOfNonTerminals + 1);
    parentProjectionsOffsets[0] = 0;

    for (uint32_t i = 0; i < numOfNonTerminals; i++)
    {
        const uint64_t next = static_cast<uint64_t>(parentProjectionsOffsets[i]) +
                              static_cast<uint64_t>(numOfParentProjections[i]);
        parentProjectionsOffsets[i + 1] = next;
    }

    std::vector<uint32_t>().swap(numOfParentProjections);

    parentProjections.assign(static_cast<size_t>(parentProjectionsOffsets[numOfNonTerminals]), 0);

    std::vector<uint64_t> cursor{};
    cursor.assign(parentProjectionsOffsets.begin(), parentProjectionsOffsets.end());

    for (uint32_t proj = 0; proj < numOfProjections; proj++)
    {
        const uint32_t a = firstNonTerminalPerProj[proj];
        if (a != unknownIndex)
        {
            parentProjections[static_cast<size_t>(cursor[a]++)] = proj;
        }

        const uint32_t b = secondNonTerminalPerProj[proj];
        if (b != unknownIndex)
        {
            parentProjections[static_cast<size_t>(cursor[b]++)] = proj;
        }
    }
}

std::shared_ptr<Cfg::OutputType> Cfg::isEmpty()
{
    TIME_MARKER("[CFG]: isEmpty");
    LOG("[CFG]: checking emptiness of CFG");

    calculateParentProjections();

    uint32_t queHead{0};
    while (queHead < generativeNonTerminalsQueue.size())
    {
        uint32_t nonTerminal{generativeNonTerminalsQueue[queHead]};
        queHead++;
        for (uint64_t it = parentProjectionsOffsets[nonTerminal];
             it < parentProjectionsOffsets[nonTerminal + 1]; it++)
        {
            const uint32_t projection = parentProjections[it];
            if (--unresolvedNonTerminalsPerProj[projection] == 0)
            {
                uint32_t nonTerminalIndex = nonTerminalIndexPerProj[projection];
                if (not isGenerative[nonTerminalIndex])
                {
                    isGenerative[nonTerminalIndex] = true;
                    witnesses[nonTerminalIndex] = {
                        .terminal = terminalValuePerProj[projection],
                        .n1Index = firstNonTerminalPerProj[projection],
                        .n2Index = secondNonTerminalPerProj[projection]};
                    generativeNonTerminalsQueue.push_back(nonTerminalIndex);
                }
            }
        }
    }

    uint32_t S = nonTerminalIndexes[static_cast<uint32_t>(NonTerminal::START)];
    if (not isGenerative[S])
    {
        return std::make_shared<OutputType>(std::nullopt);
    }

    std::vector<Terminal> example{};
    buildExample(example, S);

    return std::make_shared<OutputType>(example);
}

void Cfg::buildExample(std::vector<Terminal> &example, const uint32_t id)
{
    const Witness &witness = witnesses[id];
    if (witness.terminal != Terminal::INVALID)
    {
        example.push_back(witness.terminal);
    }
    if (witness.n1Index != unknownIndex)
    {
        buildExample(example, witness.n1Index);
    }
    if (witness.n2Index != unknownIndex)
    {
        buildExample(example, witness.n2Index);
    }
}
} // namespace teacher::cfg
