#include <iostream>
#include <string>

#include "format/API.hpp"

int main()
{
    test::format::API api;
    while (true)
    {
        std::string op;
        int arg1, arg2;
        std::cout << "podaj operacje\n";
        std::cin >> op;
        std::cout << "podaj argumenty\n";
        std::cin >> arg1 >> arg2;
        std::cout << "wynik: " << api.perform(op, arg1, arg2) << "\n";
    }
    return 0;
}
