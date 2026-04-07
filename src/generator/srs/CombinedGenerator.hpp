#include <memory>

#include "common/VPA.hpp"
#include "common/Word.hpp"
#include "generator/Generator.hpp"
#include "learner/srs/Srs.hpp"

namespace generator::srs
{
class CombinedGenerator : public Generator
{
public:
    using Generator::Generator;

    std::shared_ptr<common::VPA<AutomatonKind::Normal>> run() override;
    learner::srs::Srs getSrs() override;
    bool generatorSpecificCheck(
        std::shared_ptr<common::VPA<AutomatonKind::Normal>> hypothesis) override;

private:
    void validateGeneratorConfig(
        uint16_t &numOfStates_, uint16_t &numOfCalls_, uint16_t &numOfLocals_,
        uint16_t &numOfReturns_, uint16_t &numOfStackSymbols_) override;

    void splitAplhabet();

    template <typename Predicate>
    std::shared_ptr<common::VPA<AutomatonKind::Normal>> generateVpa(const Predicate &isVoidSymbol);

    template <typename Predicate>
    void generateTransition(const Predicate &isVoidSymbol);

    template <typename Predicate>
    void addCalls(common::transition::State state, const Predicate &isVoidSymbol);

    template <typename Predicate>
    void addLocals(common::transition::State state, const Predicate &isVoidSymbol);

    template <typename Predicate>
    void addReturns(common::transition::State state, const Predicate &isVoidSymbol);

    void addVoidCall(const common::transition::State state, const uint16_t callId);
    void addVoidReturn(
        const common::transition::State state, const uint16_t stackSymbolId,
        const uint16_t returnId);
    void addVoidLocal(const common::transition::State state, const uint16_t localId);

    bool firstVpaPredicate(const common::Symbol &symbol);
    bool secondVpaPredicate(const common::Symbol &symbol);

    uint16_t callSplitPoint{1};
    uint16_t returnSplitPoint{1};
    uint16_t localSplitPoint{1};
};
} // namespace generator