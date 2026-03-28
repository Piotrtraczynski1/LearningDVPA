#pragma once

#include <memory>
#include <vector>

#include "common/VPA.hpp"
#include "common/symbol/Symbols.hpp"
#include "common/transition/State.hpp"
#include "generator/Generator.hpp"
#include "utils/Constants.hpp"

namespace generator
{
class CdaGenerator : public Generator
{
public:
    using Generator::Generator;

    std::shared_ptr<common::VPA<AutomatonKind::Normal>> run() override final;

protected:
    uint16_t randRange(const uint16_t a, const uint16_t b) const;

    void clear();
    virtual void validateGeneratorConfig();

    virtual void selectTargetsForCalls();

    virtual common::symbol::StackSymbol selectStackSymbol(
        const uint16_t module, const uint16_t callId) const;
    virtual common::transition::State findSuccessorForReturn(const uint16_t stackSymbol) const;

    common::transition::State target[utils::MaxNumOfLetters];
    uint16_t stateToModule[utils::MaxNumOfAutomatonStates];
    std::vector<std::vector<uint16_t>> modules{};

private:
    void selectModules();
    std::vector<uint16_t> selectSplitPoints();

    common::symbol::StackSymbol encodeStackSymbol(const uint16_t module, const uint16_t callId)
        const; // TODO: maybe it's worth to have CallerModule, callId
    std::pair<uint16_t, uint16_t> decodeStackSymbol(uint16_t stackSymbol) const;

    void generateTransition();

    void addCalls();
    void addLocals();
    void addReturns();
};
} // namespace generator