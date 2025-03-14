#include "common/VPA.hpp"
#include "learner/Learner.hpp"
// #include "parser/Parser.hpp"
#include "generator/Tester.hpp"
#include "teacher/Teacher.hpp"
#include "utils/TimeMarker.hpp"

std::string findPath()
{
    std::string path = std::string(__FILE__);
    path = path.substr(0, path.find_last_of("/\\") + 1);
    // path += "parser/inputs/input.json";
    path += "parser/inputs/input2.json";
    // path += "parser/inputs/OnlyLocal.json";
    return path;
}

int main()
{
    generator::Tester tester{1000};
    tester.run();
}