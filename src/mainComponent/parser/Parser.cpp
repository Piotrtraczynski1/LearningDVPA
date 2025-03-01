#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

#include "Parser.hpp"

// TODO: refactor
namespace mainComponent::parser
{
constexpr uint8_t callId{0};
constexpr uint8_t returnId{1};
constexpr uint8_t localId{2};

common::Word Parser::parseString(std::string &word)
{
    common::Word parsedWord;
    for (const auto symbol : word)
    {
        switch (alphabet[std::string{symbol}].second)
        {
        case callId:
        {
            parsedWord.push_back({common::symbol::CallSymbol{alphabet[std::string{symbol}].first}});
            break;
        }
        case returnId:
        {
            parsedWord.push_back(
                {common::symbol::ReturnSymbol{alphabet[std::string{symbol}].first}});
            break;
        }
        case localId:
        {
            parsedWord.push_back(
                {common::symbol::LocalSymbol{alphabet[std::string{symbol}].first}});
            break;
        }
        }
    }

    return parsedWord;
}

std::shared_ptr<common::VPA> Parser::readVPA(std::string &path)
{
    readData(path);

    readAlphabetSymbol("Calls", callId);
    readAlphabetSymbol("Returns", returnId);
    readAlphabetSymbol("Locals", localId);

    readStates();
    readStackSymbols();

    readTransition();

    tran = common::transition::Transition{callT, returnT, localT};
    common::transition::State initialState{states[jsonData["Initial_state"]]};
    return std::make_shared<common::VPA>(tran, initialState);
}

void Parser::addCallTransition(json &tran)
{
    std::string symbol{tran["from"]["symbol"]};
    std::string state{tran["from"]["state"]};
    std::string stackSymbol{tran["from"]["stackSymbol"]};

    Argument<common::symbol::CallSymbol> arg{common::symbol::CallSymbol{alphabet[symbol].first},
                                             stackSymbols[stackSymbol], states[state]};

    callT[arg] = CoArgument{stackSymbols[tran["to"]["stackSymbol"]], states[tran["to"]["state"]]};
}

void Parser::addReturnTransition(json &tran)
{
    std::string symbol{tran["from"]["symbol"]};
    std::string state{tran["from"]["state"]};
    std::string stackSymbol{tran["from"]["stackSymbol"]};

    Argument<common::symbol::ReturnSymbol> arg{common::symbol::ReturnSymbol{alphabet[symbol].first},
                                               stackSymbols[stackSymbol], states[state]};

    returnT[arg] = states[tran["to"]["state"]];
}

void Parser::addLocalTransition(json &tran)
{
    std::string symbol{tran["from"]["symbol"]};
    std::string state{tran["from"]["state"]};
    std::string stackSymbol{tran["from"]["stackSymbol"]};

    Argument<common::symbol::LocalSymbol> arg{common::symbol::LocalSymbol{alphabet[symbol].first},
                                              stackSymbols[stackSymbol], states[state]};

    localT[arg] = states[tran["to"]["state"]];
}

void Parser::readTransition()
{
    for (auto &tran : jsonData["Transition"])
    {
        const std::string symbol{tran["from"]["symbol"]};
        switch (alphabet[symbol].second)
        {
        case callId:
        {
            addCallTransition(tran);
            break;
        }
        case returnId:
        {
            addReturnTransition(tran);
            break;
        }
        case localId:
        {
            addLocalTransition(tran);
            break;
        }
        }
    }
}

void Parser::readStackSymbols()
{
    uint16_t it = 0;
    for (auto &stackS : jsonData["StackSymbols"])
    {
        stackSymbols[stackS] = common::symbol::StackSymbol{it};
        it++;
    }
    stackSymbols["BOTTOM"] = common::symbol::StackSymbol::BOTTOM;
}

void Parser::readStates()
{
    uint16_t it = 0;
    for (auto &state : jsonData["States"])
    {
        states[state["name"]] = common::transition::State{it, state["accepting"]};
        it++;
    }
}

void Parser::readData(std::string &path)
{
    std::ifstream file(path);
    if (!file)
    {
        throw std::invalid_argument("invalida path to read automata");
    }
    file >> jsonData;
}

void Parser::readAlphabetSymbol(const std::string &symbol, const uint16_t id)
{
    uint16_t it = 0;
    for (auto &call : jsonData[symbol])
    {
        alphabet[call] = {it, id};
        it++;
    }
}
} // namespace mainComponent::parser
