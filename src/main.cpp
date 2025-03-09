#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void getCommandAndArguments(std::string &              input,
                            std::vector<std::string> & caa);

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
        getCommandAndArguments(input, command_and_arguments);
        if (command_and_arguments[0] == "exit")
            std::exit(command_and_arguments[1][0] - '0');

        std::cout << input << ": command not found" << '\n';
    }

    return 0;
}

void getCommandAndArguments(std::string & input, std::vector<std::string> & caa)
{
    caa.clear();

    std::string        temp("");
    std::istringstream iss(input);

    while (iss >> temp) caa.push_back(temp);

    return;
}
