#pragma once

#include "generator/callDriven/CdaGenerator.hpp"
#include "utils/Constants.hpp"

namespace generator
{
class MeVpaGenerator : public CdaGenerator
{
public:
    using CdaGenerator::CdaGenerator;

    void validateGeneratorConfig(
        [[maybe_unused]] uint16_t &numOfStates_, [[maybe_unused]] uint16_t &numOfCalls_,
        [[maybe_unused]] uint16_t &numOfLocals_, [[maybe_unused]] uint16_t &numOfReturns_,
        [[maybe_unused]] uint16_t &numOfStackSymbols_) override;

    common::symbol::StackSymbol selectStackSymbol(
        const uint16_t module, [[maybe_unused]] const uint16_t callId) const override;
    common::transition::State findSuccessorForReturn(const uint16_t stackSymbol) const override;
};
} // namespace generator
