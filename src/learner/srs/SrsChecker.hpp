#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "learner/srs/SpecialSymbolAdder.hpp"
#include "learner/srs/Srs.hpp"
#include "learner/srs/WellMatchedNode.hpp"
#include "teacher/Teacher.hpp"

namespace learner::srs
{
class SrsChecker
{
    const Srs srs;
    const teacher::Teacher &oracle;
    std::shared_ptr<SpecialSymbolAdder> specialSymbolAdder;
    std::unordered_map<uint16_t, SrsRule> specialSymbolToRule;

    const uint16_t numOfCalls;
    const uint16_t numOfReturns;
    const uint16_t numOfLocals;
    const uint16_t numOfStackSymbols;
    uint16_t numOfStates{};

    std::vector<common::Word> wellMatchedWords{};
    std::vector<WellMatchedNode> nodes{};
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

    uint16_t calcDist(const std::vector<uint16_t> &first, const std::vector<uint16_t> &second);
    void calcScore(WellMatchedNode &newNode);
    WellMatchedNode makeIdentityNode();
    bool tryInsertNode(WellMatchedNode &newNode);

    WellMatchedNode makeLocalNode(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const uint16_t localId);
    WellMatchedNode wrapNode(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const WellMatchedNode &node,
        const uint16_t callId, const uint16_t returnId);
    WellMatchedNode composeNodes(
        const WellMatchedNode &firstNode, const WellMatchedNode &secondNode);

    common::Word checkEquivalence(
        const std::shared_ptr<ConvertedAutomata> convertedAutomata, const uint16_t specialSymbol);
    common::Word prepareCounterexample(
        const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis,
        const common::Word &word);
};
} // namespace learner::srs
