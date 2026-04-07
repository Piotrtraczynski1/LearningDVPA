#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "learner/srs/AutomataConverter.hpp"
#include "learner/srs/Srs.hpp"
#include "teacher/Teacher.hpp"

namespace learner::srs
{
class SrsChecker
{
    static constexpr uint16_t MaxNumOfRevealedNodes{100};
    static constexpr uint16_t MaxNumOfNodes{40};

    struct Node
    {
        std::vector<uint16_t> nextState;
        common::Word word;
        uint16_t score;
    };

    const Srs srs;
    const teacher::Teacher &oracle;
    std::shared_ptr<AutomataConverter> converter;
    std::unordered_map<uint16_t, SrsRule> specialSymbolToRule;

    const uint16_t numOfCalls;
    const uint16_t numOfReturns;
    const uint16_t numOfLocals;
    const uint16_t numOfStackSymbols;
    uint16_t numOfStates{};

    std::vector<common::Word> wellMatchedWords{};
    std::vector<Node> nodes{};
    uint16_t minScore{};

    const common::Word emptyWord{};

public:
    SrsChecker(
        const Srs srsArg, const teacher::Teacher &oracleArg, const uint16_t numOfCallsArg,
        const uint16_t numOfReturnArg, const uint16_t numOfLocalsArg,
        const uint16_t numOfStackSymbolsArg);

    common::Word check(std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis);

private:
    uint16_t buildConvertedAutomata();
    common::Word checkConfigurationsConsistency(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis,
        const common::Word &lhs, const common::Word &rhs) const;

    void prepareWellMatchedWords(std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis);

    Node makeLocalNode(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const uint16_t localId);
    Node wrapNode(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const Node &node,
        const uint16_t callId, const uint16_t returnId);
    Node composeNodes(const Node &firstNode, const Node &secondNode);

    common::Word checkEquivalence(
        const std::shared_ptr<ConvertedAutomata> convertedAutomata, const uint16_t specialSymbol);
    common::Word prepareCounterexample(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis,
        const common::Word &word);

    uint16_t calcDist(const std::vector<uint16_t> &first, const std::vector<uint16_t> &second);
    void calcScore(Node &newNode);
    Node makeIdentityNode();
    bool tryInsertNode(Node &newNode);
};
} // namespace learner::srs
