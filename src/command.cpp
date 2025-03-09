#include "command.h"
#include <cstdlib>
#include <iostream>
#include <sstream>

Command::Command(const std::string & line_command)
{
    std::istringstream iss(line_command);
    std::string        argument;

    arguments.clear();
    iss >> command;
    while (iss >> argument) arguments.push_back(argument);

    arguments.push_back("");
}

Command::Command()
{
    command = "";
    arguments.clear();
}

void Command::ExecCommand()
{
    if (command == "exit")
        return exit();
    else
        std::cout << command << ": command not found" << '\n';

    return;
}

void Command::exit()
{
    int result;

    try
    {
        result = arguments[0].empty() ? 0 : std::stoi(arguments[0]);
    }
    catch (const std::exception &)
    {
        result = -1;
    }

    std::exit(result);

    return;
}
