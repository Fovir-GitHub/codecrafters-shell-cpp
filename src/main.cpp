#include "command.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true)
    {
        std::cout << "$ ";

        std::string input;
        std::getline(std::cin, input);

        Command command(input);
        command.ExecCommand();
    }

    return 0;
}
