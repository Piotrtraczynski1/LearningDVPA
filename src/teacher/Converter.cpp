#include "teacher/Converter.hpp"

#include "utils/TimeMarker.hpp"
#include "utils/log.hpp"

namespace teacher
{
std::unique_ptr<VPA<AutomatonKind::Combined>> Converter::combineVPA(
    const VPA<AutomatonKind::Normal> &secondVpa)
{
    return combiner.combineVPA(secondVpa);
}

std::shared_ptr<common::Word> Converter::convertVpaToCfg(const VPA<AutomatonKind::Combined> &vpa)
{
    TIME_MARKER("[Converter]: convertVpaToCfg");
    LOG("[Converter]: converting VPA to CFG");

    return std::make_shared<common::Word>(emptinessChecker.check(
        std::make_shared<VPA<AutomatonKind::Combined>>(vpa), numOfStackSymbols));
}

std::tuple<size_t, size_t> Converter::calculateEstimatedCfgSize()
{
    const uint16_t stackSymbolsNumber{
        static_cast<uint16_t>((numOfStackSymbols + 1) * (numOfStackSymbols + 1))};

    const size_t projCalls =
        static_cast<size_t>(combinedVpaNumOfStates) * static_cast<size_t>(stackSymbolsNumber) *
        static_cast<size_t>(numOfCalls) * static_cast<size_t>(combinedVpaNumOfStates) *
        static_cast<size_t>(combinedVpaNumOfStates);

    const size_t projLocals =
        static_cast<size_t>(combinedVpaNumOfStates) * static_cast<size_t>(stackSymbolsNumber) *
        static_cast<size_t>(numOfLocals) * static_cast<size_t>(combinedVpaNumOfStates);

    const size_t projReturnsBottom = static_cast<size_t>(combinedVpaNumOfStates) *
                                     static_cast<size_t>(combinedVpaNumOfStates) *
                                     static_cast<size_t>(numOfReturns + 1);

    const size_t projReturnsOther = static_cast<size_t>(combinedVpaNumOfStates) *
                                    static_cast<size_t>(stackSymbolsNumber - 1) *
                                    static_cast<size_t>(numOfReturns + 1);

    const size_t projReturns = projReturnsBottom + projReturnsOther;

    const size_t projCommon =
        static_cast<size_t>(combinedVpaNumOfStates) * static_cast<size_t>(stackSymbolsNumber) +
        static_cast<size_t>(combinedVpaNumOfStates);

    const size_t estimatedNumberOfProductions =
        (projCalls + projLocals + projReturns + projCommon) / 4 * 3;

    const size_t estimatedNonTerminals =
        (static_cast<size_t>(combinedVpaNumOfStates + 1) * static_cast<size_t>(stackSymbolsNumber) *
             static_cast<size_t>(combinedVpaNumOfStates + 1) +
         1) /
        4 * 3;

    return {estimatedNumberOfProductions, estimatedNonTerminals};
}

} // namespace teacher
