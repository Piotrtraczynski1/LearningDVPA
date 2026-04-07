#pragma once

#include "generator/callDriven/CdaGenerator.hpp"

namespace generator::callDriven
{
class ECdaGenerator : public CdaGenerator
{
public:
    using CdaGenerator::CdaGenerator;

    void validateGeneratorConfig(
        uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_,
        uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_) override;
    void selectTargetsForCalls() override;

    common::symbol::StackSymbol selectStackSymbol(
        const uint16_t module, [[maybe_unused]] const uint16_t callId) const override;
    common::transition::State findSuccessorForReturn(const uint16_t stackSymbol) const override;
};
} // namespace generator
