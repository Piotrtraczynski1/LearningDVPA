#pragma once

#include <bitset>
#include <memory>
#include <queue>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "common/symbol/Symbols.hpp"
#include "teacher/AutomataCombiner.hpp"
#include "teacher/EmptinessChecker.hpp"
#include "utils/Constants.hpp"

using namespace common;

namespace teacher
{
class Converter
{
    friend std::shared_ptr<common::Word> equivalenceCheck(
        std::shared_ptr<teacher::Converter> converter,
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> vpa);

    const std::shared_ptr<VPA<AutomatonKind::Normal>> vpa;

    AutomataCombiner combiner;
    EmptinessChecker emptinessChecker;

public:
    Converter(
        const std::shared_ptr<VPA<AutomatonKind::Normal>> &vpa, uint16_t numCallsArg,
        uint16_t numReturnsArg, uint16_t numLocalsArg, uint16_t stackSymbolsNumber)
        : vpa{vpa}, numOfCalls{numCallsArg}, numOfReturns{numReturnsArg}, numOfLocals{numLocalsArg},
          numOfStackSymbols{stackSymbolsNumber},
          combiner{vpa, numCallsArg, numReturnsArg, numLocalsArg, stackSymbolsNumber},
          emptinessChecker{numCallsArg, numReturnsArg, numLocalsArg} {};

    std::unique_ptr<VPA<AutomatonKind::Combined>> combineVPA(
        const VPA<AutomatonKind::Normal> &secondVpa);
    std::shared_ptr<common::Word> convertVpaToCfg(const VPA<AutomatonKind::Combined> &vpa);

private:
    std::tuple<size_t, size_t> calculateEstimatedCfgSize();

    uint16_t numOfCalls;
    uint16_t numOfReturns;
    uint16_t numOfLocals;
    uint16_t numOfStackSymbols;

    uint16_t combinedVpaNumOfStates;
    uint16_t combinedVpaNumOfStackSymbols;
};
} // namespace teacher
