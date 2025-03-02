#pragma once

#include "parser/Parser.hpp"
#include "../common/VPA.hpp"

namespace mainComponent
{
class mainComponent
{
    teacher::Teacher teacher;
    learner::Learner learner;
    parser::Parser parser;

public:
    void init()
    {
        
        teacher.init()
    }
};
}