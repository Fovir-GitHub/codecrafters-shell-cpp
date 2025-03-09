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

        std::string              input;
        std::vector<std::string> command_and_arguments;
        std::getline(std::cin, input);

        std::cout << input << ": command not found" << '\n';
    }

    return 0;
}
