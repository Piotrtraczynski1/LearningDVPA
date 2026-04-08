#pragma once

#include <array>
#include <memory>
#include <variant>
#include <vector>

#include "common/Stack.hpp"
#include "common/Word.hpp"
#include "common/transition/AutomatonSize.hpp"
#include "symbol/Symbols.hpp"
#include "transition/Transition.hpp"

namespace teacher
{
template <AutomatonKind Kind>
class AutomataCombiner;
class EmptinessChecker;
} // namespace teacher

namespace learner::srs
{
class SpecialSymbolAdder;
class SrsChecker;
} // namespace learner::srs

namespace common
{
template <AutomatonKind Kind = AutomatonKind::Normal>
class VPA
{
    friend class teacher::AutomataCombiner<AutomatonKind::Normal>;
    friend class teacher::AutomataCombiner<AutomatonKind::Combined>;
    friend class teacher::EmptinessChecker;
    friend class learner::srs::SrsChecker;
    friend class learner::srs::SpecialSymbolAdder;

    using Size = AutomatonSize<Kind>;

public:
    Stack stack;
    transition::State state;
    std::array<bool, Size::MaxNumOfStates> acceptingStates;

    VPA(std::unique_ptr<transition::Transition<Kind>> transition, transition::State initial,
        const std::vector<uint16_t> &accStates, uint16_t numStates);
    VPA(const VPA<Kind> &vpa);
    VPA &operator=(const VPA &vpa);
    VPA(VPA &&) = default;
    VPA &operator=(VPA &&) = default;

    bool checkWord(const Word &word);

    transition::State getSink() const;
    uint16_t getNumOfStates() const;

    void print(std::ostream &os = std::cout) const;
    void printUt(std::ostream &os = std::cout) const;

private:
    void setInitialState();
    void readWord(const Word &word);

    std::unique_ptr<transition::Transition<Kind>> delta;
    transition::State initialState;
    transition::State sink;

    uint16_t numOfStates;
};
} // namespace common