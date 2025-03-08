#include "common/VPA.hpp"
#include "learner/Learner.hpp"
#include "parser/Parser.hpp"
#include "teacher/Teacher.hpp"

std::string findPath()
{
    std::string path = std::string(__FILE__);
    path = path.substr(0, path.find_last_of("/\\") + 1);
    path += "parser/inputs/input2.json";
    // path += "parser/inputs/OnlyLocal.json";
    return path;
}

int main()
{
    mainComponent::parser::Parser parser;
    std::string path{findPath()};
    teacher::Teacher teacher{parser.readVPA(path)};
    learner::Learner learner{
        teacher, parser.numOfCallSymbols, parser.numOfReturnSymbols, parser.numOfLocalSymbols,
        parser.numOfStackSymbols};

    std::shared_ptr<common::VPA> vpa = learner.run();
    // vpa->print(); problem with a dangling reference
}