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

Cfg::Cfg(const size_t estimatedNumberOfProductions, const size_t estimatedNumberOfNonTerminals)
{
    unresolvedNonTerminalsPerProj.reserve(estimatedNumberOfProductions);
    nonTerminalIndexPerProj.reserve(estimatedNumberOfProductions);
    firstNonTerminalPerProj.reserve(estimatedNumberOfProductions);
    secondNonTerminalPerProj.reserve(estimatedNumberOfProductions);
    terminalValuePerProj.reserve(estimatedNumberOfProductions);

    nonTerminalIndexes.reset(new uint32_t[utils::MaxNumOfNonTerminals]());
    generativeNonTerminalsQueue.reserve(estimatedNumberOfNonTerminals);
    isGenerative.reserve(estimatedNumberOfNonTerminals);
    witnesses.reserve(estimatedNumberOfNonTerminals);
    numOfParentProductions.reserve(estimatedNumberOfNonTerminals);

    insertDummyElements();
}

void Cfg::insertDummyElements()
{
    isGenerative.push_back(false);
    witnesses.push_back(Witness{});
    numOfParentProductions.push_back(0);
}

void Cfg::addProduction(
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
        numOfParentProductions[n1Index]++;
        deps++;
    }
    uint32_t n2Index{unknownIndex};
    if (n2 != NonTerminal::INVALID)
    {
        n2Index = setNonTerminalIndex(n2);
        numOfParentProductions[n2Index]++;
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

    numOfProductions++;

    if (numOfProductions == unknownIndex)
    {
        ERR("[CFG]: Too many productions!");
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

void Cfg::calculateParentProductions()
{
    TIME_MARKER("[CFG]: calculateParentProductions");

    parentProductionsOffsets.resize(numOfNonTerminals + 1);
    parentProductionsOffsets[0] = 0;

    for (uint32_t i = 0; i < numOfNonTerminals; i++)
    {
        const uint64_t next = static_cast<uint64_t>(parentProductionsOffsets[i]) +
                              static_cast<uint64_t>(numOfParentProductions[i]);
        parentProductionsOffsets[i + 1] = next;
    }

    std::vector<uint32_t>().swap(numOfParentProductions);

    parentProductions.assign(static_cast<size_t>(parentProductionsOffsets[numOfNonTerminals]), 0);

    std::vector<uint64_t> cursor{};
    cursor.assign(parentProductionsOffsets.begin(), parentProductionsOffsets.end());

    for (uint32_t proj = 0; proj < numOfProductions; proj++)
    {
        const uint32_t a = firstNonTerminalPerProj[proj];
        if (a != unknownIndex)
        {
            parentProductions[static_cast<size_t>(cursor[a]++)] = proj;
        }

        const uint32_t b = secondNonTerminalPerProj[proj];
        if (b != unknownIndex)
        {
            parentProductions[static_cast<size_t>(cursor[b]++)] = proj;
        }
    }
}

std::shared_ptr<Cfg::OutputType> Cfg::isEmpty()
{
    TIME_MARKER("[CFG]: isEmpty");
    LOG("[CFG]: checking emptiness of CFG");

    calculateParentProductions();

    uint32_t queHead{0};
    while (queHead < generativeNonTerminalsQueue.size())
    {
        uint32_t nonTerminal{generativeNonTerminalsQueue[queHead]};
        queHead++;
        for (uint64_t it = parentProductionsOffsets[nonTerminal];
             it < parentProductionsOffsets[nonTerminal + 1]; it++)
        {
            const uint32_t production = parentProductions[it];
            if (--unresolvedNonTerminalsPerProj[production] == 0)
            {
                uint32_t nonTerminalIndex = nonTerminalIndexPerProj[production];
                if (not isGenerative[nonTerminalIndex])
                {
                    isGenerative[nonTerminalIndex] = true;
                    witnesses[nonTerminalIndex] = {
                        .terminal = terminalValuePerProj[production],
                        .n1Index = firstNonTerminalPerProj[production],
                        .n2Index = secondNonTerminalPerProj[production]};
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
