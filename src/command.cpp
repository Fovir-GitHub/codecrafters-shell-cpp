#include "command.h"
#include <cstdlib>
#include <iostream>
#include <sstream>

Command::Command(const std::string & line_command)
{
    std::istringstream iss(line_command);
    std::string        argument;

    arguments.clear();
    iss >> command; /* Get the command. */
    while (iss >> argument)
        arguments.push_back(argument); /* Get the arguments. */

    arguments.push_back(""); /* Add an empty argument as the final argument. */
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
        // If the argument is empty, return 0 by default.
        result = arguments[0].empty() ? 0 : std::stoi(arguments[0]);
    }
    catch (const std::exception &)
    {
        result = -1; /* Exception situation */
    }

    std::exit(result);

    return;
}
