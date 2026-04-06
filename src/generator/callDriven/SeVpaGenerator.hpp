#pragma once

#include "generator/callDriven/CdaGenerator.hpp"
#include "utils/Constants.hpp"

namespace generator
{
class SeVpaGenerator : public CdaGenerator
{
public:
    using CdaGenerator::CdaGenerator;

    void selectTargetsForCalls() override;

    void validateGeneratorConfig(
        [[maybe_unused]] uint16_t &numOfStates_, [[maybe_unused]] uint16_t &numOfCalls_,
        [[maybe_unused]] uint16_t &numOfLocals_, [[maybe_unused]] uint16_t &numOfReturns_,
        [[maybe_unused]] uint16_t &numOfStackSymbols_) override;

private:
    void selectEntryPoints();

    uint16_t entryPoints[utils::MaxNumOfCombinedAutomatonLetters];
};
} // namespace generator
