#include "command.h"
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
