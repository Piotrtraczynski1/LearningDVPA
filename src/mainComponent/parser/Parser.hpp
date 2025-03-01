// Class just for testing purposes

#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../../common/VPA.hpp"
#include "../../common/Word.hpp"
#include "../../common/transition/Argument.hpp"
#include "../../common/transition/CoArgument.hpp"

namespace mainComponent::parser
{
class Parser
{
    using json = nlohmann::json;
    template <typename T> using Argument = common::transition::Argument<T>;
    using CoArgument = common::transition::CoArgument;
public:
    std::shared_ptr<common::VPA> readVPA(std::string &path);
    std::vector<common::Symbol> parseString(std::string &word);

private:
    void readData(std::string &path);
    void readAlphabetSymbol(const std::string &symbol, const uint16_t id);
    void readStates();
    void readStackSymbols();
    void readTransition();
    void addCallTransition(json &tran);
    void addReturnTransition(json &tran);
    void addLocalTransition(json &tran);

    json jsonData;
    std::map<std::string, std::pair<uint16_t, uint8_t>> alphabet;
    std::map<std::string, common::transition::State> states;
    std::map<std::string, common::symbol::StackSymbol> stackSymbols;

    std::map<Argument<common::symbol::CallSymbol>, CoArgument> callT;
    std::map<Argument<common::symbol::ReturnSymbol>, common::transition::State> returnT;
    std::map<Argument<common::symbol::LocalSymbol>, common::transition::State> localT;
    common::transition::Transition tran;
};
} // namespace mainComponent::parser