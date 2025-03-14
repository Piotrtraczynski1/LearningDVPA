#include "teacher/cfg/Cfg.hpp"
#include "common/Word.hpp"
#include "teacher/cfg/Calculator.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace teacher::cfg
{
Cfg::Cfg(const size_t estimatedNumberOfProjections, const size_t estimatedNumberOfNonTerminals)
{
    projections.reserve(estimatedNumberOfProjections);
    unresolved.reserve(estimatedNumberOfProjections);

    uses.reserve(estimatedNumberOfNonTerminals);
    gen.reserve(estimatedNumberOfNonTerminals);
    witnesses.reserve(estimatedNumberOfNonTerminals);
}

void Cfg::addProjection(
    NonTerminal nonTerminal, std::optional<Terminal> terminal, std::optional<NonTerminal> n1,
    std::optional<NonTerminal> n2)
{
    TIME_MARKER("addProjection");
    projections.push_back({nonTerminal, terminal, n1, n2});

    const auto nonTerminalId = setNonTerminalIndex(nonTerminal);

    uint8_t deps{0};
    if (n1.has_value())
    {
        uses[setNonTerminalIndex(n1.value())].push_back(numOfProjections);
        deps++;
    }
    if (n2.has_value())
    {
        uses[setNonTerminalIndex(n2.value())].push_back(numOfProjections);
        deps++;
    }

    unresolved.push_back(deps);

    if (deps == 0 and not gen[nonTerminalId])
    {
        gen[nonTerminalId] = true;
        witnesses[nonTerminalId] = numOfProjections;
        que.push(nonTerminalId);
    }
    numOfProjections++;
}

uint64_t Cfg::setNonTerminalIndex(const NonTerminal nonTerminal)
{
    TIME_MARKER("setNonTerminalIndex");
    auto it = nonTerminalIndexes.find(nonTerminal);
    if (it != nonTerminalIndexes.end())
    {
        return it->second;
    }

    const uint64_t nonTerminalId{numOfNonTerminals};
    numOfNonTerminals++;

    nonTerminalIndexes.emplace(nonTerminal, nonTerminalId);
    gen.push_back(false);
    witnesses.push_back(0);
    uses.push_back({});
    constexpr uint8_t tmp{100}; // TODO
    uses.back().reserve(tmp);

    return nonTerminalId;
}

std::shared_ptr<Cfg::OutputType> Cfg::isEmpty()
{
    TIME_MARKER("CFG.isEmpty()");

    if (nonTerminalIndexes.count(NonTerminal::START) == 0)
        return std::make_shared<OutputType>(std::nullopt);

    while (not que.empty())
    {
        uint64_t id{que.front()};
        que.pop();
        for (uint64_t projection : uses[id])
        {
            unresolved[projection]--;
            if (unresolved[projection] == 0)
            {
                uint64_t nonTerminalIndex = nonTerminalIndexes[projections[projection].arg];
                if (!gen[nonTerminalIndex])
                {
                    gen[nonTerminalIndex] = 1;
                    witnesses[nonTerminalIndex] = projection;
                    que.push(nonTerminalIndex);
                }
            }
        }
    }

    int S = nonTerminalIndexes[NonTerminal::START];
    if (!gen[S])
        return std::make_shared<OutputType>(std::nullopt);

    std::vector<Terminal> example{};
    buildExample(example, S);

    return std::make_shared<OutputType>(example);
}

void Cfg::buildExample(std::vector<Terminal> &example, const uint64_t id)
{
    const uint64_t projectionId = witnesses[id];
    const auto &pr = projections[projectionId];
    if (pr.terminal.has_value())
    {
        example.push_back(pr.terminal.value());
    }
    if (pr.n1.has_value())
    {
        buildExample(example, nonTerminalIndexes[pr.n1.value()]);
    }
    if (pr.n2.has_value())
    {
        buildExample(example, nonTerminalIndexes[pr.n2.value()]);
    }
}

namespace
{
std::ostream &operator<<(std::ostream &os, const std::optional<cfg::Terminal> &opt)
{
    if (opt.has_value())
    {
        os << Calculator::convertTerminalToSymbol(opt.value());
        return os;
    }

    os << "null";
    return os;
}

void printNonTerminal(std::ostream &os, cfg::NonTerminal nt)
{
    uint64_t q1 = (nt / (1ULL << 34)) - 1;
    uint64_t sId = ((nt % (1ULL << 34)) / (1ULL << 17)) - 1;
    uint64_t q2 = (nt % (1ULL << 17)) - 1;

    os << "{q1=" << q1 << ", sId=" << sId << ", q2=" << q2 << "}";
}

std::ostream &operator<<(std::ostream &os, const std::optional<cfg::NonTerminal> &opt)
{
    if (opt.has_value())
    {
        printNonTerminal(os, opt.value());
        return os;
    }

    os << "null";
    return os;
}

std::ostream &operator<<(std::ostream &os, const cfg::NonTerminal &nt)
{
    printNonTerminal(os, nt);
    return os;
}
} // namespace

void Cfg::print(std::ostream &os)
{
    for (auto projection : projections)
    {
        os << "=== NonTerminal: " << projection.arg << std::endl;
        os << "coArgu: terminal = " << projection.terminal << ", n1 = " << projection.n1
           << ", n2 = " << projection.n2 << std::endl;
    }
}
} // namespace teacher::cfg
