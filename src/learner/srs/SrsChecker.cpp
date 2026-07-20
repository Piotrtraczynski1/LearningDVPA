#include <algorithm>
#include <numeric>
#include <queue>

#include "learner/srs/ConvertedAutomata.hpp"
#include "learner/srs/SrsChecker.hpp"
#include "teacher/AutomataCombiner.hpp"
#include "teacher/EmptinessChecker.hpp"
#include "teacher/EquivalenceCheck.hpp"
#include "utils/Constants.hpp"
#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace learner::srs
{
namespace
{
bool hasOneUnmatchedCall(const common::Word &word)
{
    uint16_t nesting{0};
    for (const auto &symbol : word)
    {
        if (symbol.index() == common::CallSymbolVariant)
        {
            nesting++;
        }
        else if (symbol.index() == common::ReturnSymbolVariant)
        {
            if (nesting == 0)
            {
                return false;
            }
            nesting--;
        }
    }

    return nesting == 1;
}
} // namespace

SrsChecker::SrsChecker(
    const Srs srsArg, const teacher::Teacher &oracleArg, const uint16_t numOfCallsArg,
    const uint16_t numOfReturnArg, const uint16_t numOfLocalsArg,
    const uint16_t numOfStackSymbolsArg)
    : srs{srsArg}, oracle{oracleArg}, numOfCalls{numOfCallsArg}, numOfReturns{numOfReturnArg},
      numOfLocals{numOfLocalsArg}, numOfStackSymbols{numOfStackSymbolsArg}
{
}

common::Word SrsChecker::check(std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    if (srs.empty())
    {
        return common::Word{};
    }

    TIME_MARKER("[SrsChecker]: check consistency with SRS");
    LOG("[SrsChecker]: check consistency with SRS");

    specialSymbolAdder = std::make_shared<SpecialSymbolAdder>(hypothesis);
    specialLocalSymbolToRule.clear();
    specialCallSymbolToRule.clear();
    prepareWellMatchedWords(hypothesis);
    const ConvertedAlphabet alphabet{buildConvertedAutomata()};

    auto convertedAutomata{specialSymbolAdder->getConvertedAutomata()};
    auto word{checkEquivalence(convertedAutomata, alphabet)};

    if (word != emptyWord and seenWords[word]++ < 5)
    {
        return prepareCounterexample(hypothesis, word);
    }

    return common::Word{};
}

SrsChecker::ConvertedAlphabet SrsChecker::buildConvertedAutomata()
{
    ConvertedAlphabet alphabet{.numOfCalls = numOfCalls, .numOfLocals = numOfLocals};
    if (not addStaticRules(alphabet))
    {
        return alphabet;
    }

    addParameterizedRules(alphabet);
    return alphabet;
}

bool SrsChecker::addStaticRules(ConvertedAlphabet &alphabet)
{
    for (const auto &rule : srs)
    {
        if (rule.left.takesParams or rule.right.takesParams)
        {
            continue;
        }

        if (not addRule(instantiateRule(rule, common::Word{}), alphabet))
        {
            return false;
        }
    }

    return true;
}

bool SrsChecker::addParameterizedRules(ConvertedAlphabet &alphabet)
{
    for (const auto &word : wellMatchedWords)
    {
        for (const auto &rule : srs)
        {
            if (not(rule.left.takesParams or rule.right.takesParams))
            {
                continue;
            }

            if (not addRule(instantiateRule(rule, word), alphabet))
            {
                return false;
            }
        }
    }

    return true;
}

bool SrsChecker::addRule(const SrsRule &rule, ConvertedAlphabet &alphabet)
{
    if (hasOneUnmatchedCall(rule.left) and hasOneUnmatchedCall(rule.right))
    {
        if (alphabet.numOfCalls == utils::MaxNumOfLetters)
        {
            WRN("[SrsChecker]: max number of calls reached! Some Srs Rules will be discarded!");
            return false;
        }

        specialSymbolAdder->addNewCallRule(alphabet.numOfCalls, rule);
        specialCallSymbolToRule[alphabet.numOfCalls] = rule;
        alphabet.numOfCalls++;
        return true;
    }

    if (alphabet.numOfLocals == utils::MaxNumOfLetters)
    {
        WRN("[SrsChecker]: max number of letters reached! Some Srs Rules will be discarded!");
        return false;
    }

    specialSymbolAdder->addNewRule(alphabet.numOfLocals, rule);
    specialLocalSymbolToRule[alphabet.numOfLocals] = rule;
    alphabet.numOfLocals++;
    return true;
}

SrsRule SrsChecker::instantiateRule(const SrsRuleWithParams &rule, const common::Word &word) const
{
    return {
        .left = rule.left.left + (rule.left.takesParams ? word : common::Word{}) + rule.left.right,
        .right =
            rule.right.left + (rule.right.takesParams ? word : common::Word{}) + rule.right.right};
}

void SrsChecker::prepareWellMatchedWords(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis)
{
    wellMatchedWords.clear();
    nodes.clear();
    numOfStates = hypothesis->getNumOfStates();
    minScore = numOfStates;

    auto candidates = std::make_unique<std::queue<WellMatchedNode>>();
    candidates->push(makeIdentityNode());
    nodes.push_back(candidates->front());

    uint16_t numOfRevealedNodes{1};

    for (uint16_t localId = 0; localId < numOfLocals; localId++)
    {
        auto newNode{makeLocalNode(hypothesis, localId)};
        if (tryInsertNode(newNode))
        {
            candidates->push(newNode);
        }
    }

    while (not candidates->empty() and
           numOfRevealedNodes < utils::MaxNumOfRevealedWellMatchedWordsForSrsCheck)
    {
        WellMatchedNode node{candidates->front()};
        candidates->pop();

        for (uint16_t callId = 0; callId < numOfCalls; callId++)
        {
            for (uint16_t returnId = 0; returnId < numOfReturns; returnId++)
            {
                auto newNode{wrapNode(hypothesis, node, callId, returnId)};
                if (tryInsertNode(newNode))
                {
                    candidates->push(newNode);
                }
            }
        }

        const auto numOfNodes{nodes.size()};
        for (uint16_t nodeId = 0; nodeId < numOfNodes; nodeId++)
        {
            auto newNode{composeNodes(node, nodes[nodeId])};
            if (tryInsertNode(newNode))
            {
                candidates->push(newNode);
            }
        }

        numOfRevealedNodes++;
    }

    std::sort(
        nodes.begin(), nodes.end(), [](const WellMatchedNode &l, const WellMatchedNode &r) -> bool
        { return l.score > r.score; });

    wellMatchedWords.reserve(nodes.size());
    for (const auto &node : nodes)
    {
        wellMatchedWords.push_back(node.word);
    }

    LOG("[SrsChecker]: prepared %lu well-matched words", wellMatchedWords.size());
}

WellMatchedNode SrsChecker::makeLocalNode(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const uint16_t localId)
{
    std::vector<uint16_t> nextState(numOfStates, common::transition::State::INVALID);
    common::Word word{common::symbol::LocalSymbol{localId}};

    for (uint16_t state = 0; state < numOfStates; state++)
    {
        hypothesis->state = common::transition::State{state};
        hypothesis->readWord(word);
        nextState[state] = hypothesis->state;
    }

    return WellMatchedNode{.nextState = nextState, .word = word};
}

WellMatchedNode SrsChecker::wrapNode(
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const WellMatchedNode &node,
    const uint16_t callId, const uint16_t returnId)
{
    std::vector<uint16_t> nextState(numOfStates, common::transition::State::INVALID);
    common::Word word{
        common::Word{common::symbol::CallSymbol{callId}} + node.word +
        common::Word{common::symbol::ReturnSymbol{returnId}}};

    for (uint16_t state = 0; state < numOfStates; state++)
    {
        hypothesis->state = common::transition::State{state};
        hypothesis->readWord(word);
        nextState[state] = hypothesis->state;
    }

    return WellMatchedNode{.nextState = nextState, .word = word};
}

WellMatchedNode SrsChecker::composeNodes(
    const WellMatchedNode &firstNode, const WellMatchedNode &secondNode)
{
    std::vector<uint16_t> nextState(numOfStates, common::transition::State::INVALID);

    for (uint16_t state = 0; state < numOfStates; state++)
    {
        if (firstNode.nextState[state] == common::transition::State::INVALID)
        {
            continue;
        }
        nextState[state] = secondNode.nextState[firstNode.nextState[state]];
    }

    return WellMatchedNode{.nextState = nextState, .word = firstNode.word + secondNode.word};
}

bool SrsChecker::tryInsertNode(WellMatchedNode &newNode)
{
    calcScore(newNode);
    if (newNode.score == 0)
    {
        return false;
    }

    if (nodes.size() < utils::MaxNumOfWellMatchedWordsForSrsCheck)
    {
        nodes.push_back(newNode);
        minScore = std::min(minScore, newNode.score);
        return true;
    }

    if (minScore >= newNode.score)
    {
        return false;
    }

    for (auto &node : nodes)
    {
        if (node.score == minScore)
        {
            node = newNode;
            break;
        }
    }

    minScore = numOfStates;
    for (auto &node : nodes)
    {
        minScore = std::min(minScore, node.score);
    }
    return true;
}

uint16_t SrsChecker::calcDist(
    const std::vector<uint16_t> &first, const std::vector<uint16_t> &second)
{
    uint16_t dist{0};
    for (uint16_t state = 0; state < numOfStates; state++)
    {
        dist += (first[state] != second[state]);
    }
    return dist;
}

void SrsChecker::calcScore(WellMatchedNode &newNode)
{
    newNode.score = numOfStates;
    for (const auto &node : nodes)
    {
        newNode.score = std::min(newNode.score, calcDist(newNode.nextState, node.nextState));
    }
}

WellMatchedNode SrsChecker::makeIdentityNode()
{
    std::vector<uint16_t> next(numOfStates);
    std::iota(next.begin(), next.end(), 0);
    return WellMatchedNode{.nextState = next, .word = common::Word{}, .score = numOfStates};
}

common::Word SrsChecker::checkEquivalence(
    const std::shared_ptr<ConvertedAutomata> convertedAutomata, const ConvertedAlphabet &alphabet)
{
    auto automataCombiner = std::make_shared<teacher::AutomataCombiner<AutomatonKind::Combined>>(
        convertedAutomata->lAutomaton, alphabet.numOfCalls, numOfReturns, alphabet.numOfLocals,
        numOfStackSymbols);
    auto emptinessChecker = std::make_shared<teacher::EmptinessChecker>(
        alphabet.numOfCalls, numOfReturns, alphabet.numOfLocals, numOfStackSymbols);
    auto output = teacher::equivalenceCheck(
        automataCombiner, emptinessChecker, convertedAutomata->rAutomaton);

    common::Symbol leftoverSymbol{common::symbol::ReturnSymbol{numOfReturns}};
    for (uint16_t i = 0; i < output->size(); i++)
    {
        if ((*output)[i] == leftoverSymbol)
        {
            return common::Word{(*output).begin(), (*output).begin() + i};
        }
    }

    return *output;
}

common::Word SrsChecker::prepareCounterexample(
    const std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis, const common::Word &word)
{
    TIME_MARKER("[SrsChecker]: counterexample found");
    std::cout << "[SrsChecker]: counterexample found: " << word << std::endl;

    const auto [firstCandidate, secondCandidate]{expandCounterexample(word)};

    if (oracle.membershipQuery(firstCandidate) != hypothesis->checkWord(firstCandidate))
    {
        return firstCandidate;
    }

    return secondCandidate;
}

std::pair<common::Word, common::Word> SrsChecker::expandCounterexample(
    const common::Word &word) const
{
    common::Word firstCandidate{};
    common::Word secondCandidate{};

    for (const auto &symbol : word)
    {
        if (appendSpecialRule(symbol, firstCandidate, secondCandidate))
        {
            continue;
        }

        firstCandidate += symbol;
        secondCandidate += symbol;
    }

    return {firstCandidate, secondCandidate};
}

bool SrsChecker::appendSpecialRule(
    const common::Symbol &symbol, common::Word &firstCandidate, common::Word &secondCandidate) const
{
    const auto appendRule = [&firstCandidate, &secondCandidate](const SrsRule &rule)
    {
        firstCandidate += rule.left;
        secondCandidate += rule.right;
    };

    if (symbol.index() == common::LocalSymbolVariant)
    {
        const auto specialSymbol{
            static_cast<uint16_t>(std::get<common::symbol::LocalSymbol>(symbol))};
        const auto it{specialLocalSymbolToRule.find(specialSymbol)};
        if (it != specialLocalSymbolToRule.end())
        {
            appendRule(it->second);
            return true;
        }
    }
    else if (symbol.index() == common::CallSymbolVariant)
    {
        const auto specialSymbol{
            static_cast<uint16_t>(std::get<common::symbol::CallSymbol>(symbol))};
        const auto it{specialCallSymbolToRule.find(specialSymbol)};
        if (it != specialCallSymbolToRule.end())
        {
            appendRule(it->second);
            return true;
        }
    }

    return false;
}
} // namespace learner::srs
