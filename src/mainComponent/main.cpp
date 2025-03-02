#include "../learner/Learner.hpp"
#include "../teacher/Teacher.hpp"

int main()
{
    teacher::Teacher teacher;
    learner::Learner learner{teacher, 1, 1, 1, 1};

    learner.run();
}