#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "common/VPA.hpp"
#include "common/transition/Transition.hpp"
#include "generator/Generator.hpp"
#include "learner/Srs.hpp"

namespace generator
{
class eCDAGenerator : public Generator
{
public:
    using Generator::Generator;

    std::shared_ptr<common::VPA> run() override;
    bool generatorSpecificCheck(std::shared_ptr<common::VPA> hypothesis) override;
    learner::Srs getSrs() override;

private:
    learner::Srs srs{};
    std::vector<uint16_t> acceptingStates{0};
    std::vector<std::vector<uint16_t>> splitedStates{};
    common::transition::State initialState{0};
    std::map<uint16_t, uint16_t> stackSymbolsToModules{};
    std::vector<std::vector<uint16_t>> modulesToStackSymbols{};
    std::set<uint16_t> takenSuccessors{initialState};

    void clear();
    void splitStates();
    std::vector<uint16_t> selectSplitPoints();
    void selectStackSymbolsToModules();

    common::transition::State selectCallSuccessor();
    common::symbol::StackSymbol selectStackSymbolForModule(const uint16_t module);
    common::transition::State findSuccessorForReturn(const uint16_t stackSymbol);

    void addCalls();
    void addLocals();
    void addReturns();
    void selectAcceptingStates();
};
} // namespace generator
