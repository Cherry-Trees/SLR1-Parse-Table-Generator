#include "functions.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2)
        std::cerr << "Usage: a.exe <filename>" << std::endl;
    
    std::ifstream input(argv[1]);
    build(input);

    return 0;
};