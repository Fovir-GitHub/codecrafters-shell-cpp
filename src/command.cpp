#include "command.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ranges>
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

int Command::getCommand()
{
    if (command == "exit")
        return EXIT;
    else if (command == "echo")
        return ECHO;
    else if (command == "type")
        return TYPE;
    else if (GetFullPath(command) != "")
        return EXECUTABLE;
    else
        return ERROR;
}

void Command::ExecCommand()
{
    int command_id = getCommand();

    switch (command_id)
    {
    case EXIT:
        exit();
        break;
    case ECHO:
        echo();
        break;
    case TYPE:
        type();
        break;
    case EXECUTABLE:
    {
        std::string cmd(command);
        for (auto & s : arguments) cmd = cmd + " " + s;
        std::system(cmd.c_str());
        break;
    }
    default:
        std::cout << command << ": command not found" << '\n';
    }

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

void Command::echo()
{
    // Output all arguments.
    for (auto & s : arguments) std::cout << s << ' ';
    std::cout << '\n';

    return;
}

void Command::type()
{
    int command_id = Command(arguments[0]).getCommand();

    // The command is built in.
    if (command_id != ERROR && command_id != EXECUTABLE)
        std::cout << arguments[0] << " is a shell builtin" << '\n';
    else if (command_id == EXECUTABLE) /* The command is external */
        std::cout << arguments[0] << " is " << GetFullPath(arguments[0])
                  << '\n';
    else /* The command is error */
        std::cerr << arguments[0] << ": not found" << '\n';

    return;
}

std::string Command::GetEnvironmentVariable(const std::string & env_type)
{
    return std::getenv(env_type.c_str());
}

std::string Command::GetFullPath(std::string & cmd)
{
    std::string       env_path = GetEnvironmentVariable("PATH");
    std::stringstream ss(env_path);
    std::string       path;

    while (std::getline(ss, path, ':')) /* Split "PATH" with ':' */
    {
        std::string full_path = path + "/" + cmd;
        if (std::filesystem::exists(full_path))
            return full_path;
    }

    return "";
}
