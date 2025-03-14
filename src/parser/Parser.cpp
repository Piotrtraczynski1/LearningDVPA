#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

#include "parser/Parser.hpp"

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

    numOfCallSymbols = jsonData["Calls"].size();
    numOfReturnSymbols = jsonData["Returns"].size();
    numOfLocalSymbols = jsonData["Locals"].size();
    numOfStackSymbols = jsonData["StackSymbols"].size();

    readStates();
    readStackSymbols();

    readTransition();

    transition = common::transition::Transition{callT, returnT, localT};
    common::transition::State initialState{states[jsonData["Initial_state"]]};
    return std::make_shared<common::VPA>(transition, initialState, acceptingStates, numOfStates);
}

template <typename Symbol>
Parser::Argument<Symbol> Parser::makeArgument(json &tran)
{
    std::string symbol{tran["from"]["symbol"]};
    std::string state{tran["from"]["state"]};
    std::string stackSymbol{tran["from"]["stackSymbol"]};

    Argument<Symbol> arg{Symbol{alphabet[symbol].first}, stackSymbols[stackSymbol], states[state]};

    return arg;
}

void Parser::addCallTransition(json &tran)
{
    Argument<common::symbol::CallSymbol> arg{makeArgument<common::symbol::CallSymbol>(tran)};
    callT[arg] = CoArgument{stackSymbols[tran["to"]["stackSymbol"]], states[tran["to"]["state"]]};
}

void Parser::addReturnTransition(json &tran)
{
    Argument<common::symbol::ReturnSymbol> arg{makeArgument<common::symbol::ReturnSymbol>(tran)};
    returnT[arg] = states[tran["to"]["state"]];
}

void Parser::addLocalTransition(json &tran)
{
    Argument<common::symbol::LocalSymbol> arg{makeArgument<common::symbol::LocalSymbol>(tran)};
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
    uint16_t it = 1;
    for (auto &stackS : jsonData["StackSymbols"])
    {
        stackSymbols[stackS] = common::symbol::StackSymbol{it};
        it++;
    }
    stackSymbols["BOTTOM"] = common::symbol::StackSymbol::BOTTOM;
}

void Parser::readStates()
{
    numOfStates = jsonData["States"].size();
    uint16_t it = 0;
    for (auto &state : jsonData["States"])
    {
        states[state["name"]] = common::transition::State{it};
        if (state["accepting"])
        {
            acceptingStates.push_back(it);
        }
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
