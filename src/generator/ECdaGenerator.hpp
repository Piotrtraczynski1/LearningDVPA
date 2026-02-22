#pragma once

#include "generator/CdaGenerator.hpp"

namespace generator
{
class ECdaGenerator : public CdaGenerator
{
public:
    using CdaGenerator::CdaGenerator;

    void validateGeneratorConfig() override;
    void selectTargetsForCalls() override;

    common::symbol::StackSymbol
    selectStackSymbol(const uint16_t module, [[maybe_unused]] const uint16_t callId) const override;
    common::transition::State findSuccessorForReturn(const uint16_t stackSymbol) const override;
};
} // namespace generator
