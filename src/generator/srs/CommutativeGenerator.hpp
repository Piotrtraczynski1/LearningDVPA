#include <cstddef>
#include <memory>
#include <vector>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "generator/Generator.hpp"
#include "learner/srs/Srs.hpp"

namespace generator::srs
{
class CommutativeGenerator : public Generator
{
public:
    using Generator::Generator;

    std::shared_ptr<common::VPA<AutomatonKind::Normal>> run() override;
    learner::srs::Srs getSrs() override;
    bool generatorSpecificCheck(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis) override;

private:
    using State = common::transition::State;
    using StackSymbol = common::symbol::StackSymbol;

    struct CallTransition
    {
        State destination{State::INVALID};
        StackSymbol stackSymbol{StackSymbol::INVALID};
    };

    struct ComponentTransitions
    {
        ComponentTransitions(
            uint16_t numOfStates, uint16_t callBegin, uint16_t callEnd, uint16_t localBegin,
            uint16_t localEnd, uint16_t returnBegin, uint16_t returnEnd, uint16_t stackBegin,
            uint16_t stackEnd, uint16_t numOfCalls, uint16_t numOfLocals, uint16_t numOfReturns,
            uint16_t numOfStackSymbols);

        uint16_t numOfStates;
        uint16_t callBegin;
        uint16_t callEnd;
        uint16_t localBegin;
        uint16_t localEnd;
        uint16_t returnBegin;
        uint16_t returnEnd;
        uint16_t stackBegin;
        uint16_t stackEnd;

        std::vector<CallTransition> calls;
        std::vector<State> locals;
        std::vector<State> returns;
    };

    void validateGeneratorConfig(
        uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_,
        uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_) override;

    void splitAlphabet();
    ComponentTransitions generateComponent(
        uint16_t componentNumOfStates, uint16_t callBegin, uint16_t callEnd, uint16_t localBegin,
        uint16_t localEnd, uint16_t returnBegin, uint16_t returnEnd, uint16_t stackBegin,
        uint16_t stackEnd);
    void generateComponentTransitions(ComponentTransitions &component);
    void addCalls(ComponentTransitions &component, State state);
    void addLocals(ComponentTransitions &component, State state);
    void addReturns(ComponentTransitions &component, State state);

    void generateProduct(const ComponentTransitions &first, const ComponentTransitions &second);
    void addProductCalls(
        State source, uint16_t firstState, uint16_t secondState, const ComponentTransitions &first,
        const ComponentTransitions &second);
    void addProductLocals(
        State source, uint16_t firstState, uint16_t secondState, const ComponentTransitions &first,
        const ComponentTransitions &second);
    void addProductReturns(
        State source, uint16_t firstState, uint16_t secondState, const ComponentTransitions &first,
        const ComponentTransitions &second);

    void selectProductAcceptingStates(uint16_t firstNumOfStates, uint16_t secondNumOfStates);
    std::vector<bool> selectComponentAcceptingStates(uint16_t componentNumOfStates);

    size_t callIndex(uint16_t state, uint16_t call) const;
    size_t localIndex(uint16_t state, uint16_t local) const;
    size_t returnIndex(uint16_t state, uint16_t stackSymbol, uint16_t ret) const;
    State combineStates(uint16_t first, uint16_t second) const;

    uint16_t callSplitPoint{1};
    uint16_t returnSplitPoint{1};
    uint16_t localSplitPoint{1};
    uint16_t stackSplitPoint{2};
};
} // namespace generator::srs
