#include "command.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <sstream>

void Command::FallBack()
{
    std::cout << command << ": command not found" << '\n';
}

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
    if (command_map.find(command) != command_map.end())
        command_map[command]();
    else
        command_map["external"]();

    return;
}

void Command::RunExternalCommand()
{
    std::string full_path = GetFullPath(command);
    if (full_path == "")
        FallBack();
    else
    {
        std::string full_command(command);
        for (auto & arg : arguments) full_command += (" " + arg);
        std::system(full_command.c_str());
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
    std::string temp_full_path = GetFullPath(arguments[0]);

    // The command is built in.
    if (command_map.find(arguments[0]) != command_map.end())
        std::cout << arguments[0] << " is a shell builtin" << '\n';
    else if (temp_full_path != "") /* The command is external */
        std::cout << arguments[0] << " is " << temp_full_path << '\n';
    else /* The command is error */
        std::cerr << arguments[0] << ": not found" << '\n';

    return;
}

void Command::pwd()
{
    std::cout << std::filesystem::current_path().string() << '\n';

    return;
}

void Command::cd()
{
    namespace fs = std::filesystem;

    fs::path new_dir = arguments[0];
    if (fs::exists(new_dir) && fs::is_directory(new_dir))
        fs::current_path(new_dir);
    else
        std::cerr << "cd: " << new_dir << ": No such file or directory" << '\n';

    return;
}

std::string Command::GetEnvironmentVariable(const std::string & env_type)
{
    return std::getenv(env_type.c_str());
}

std::string Command::GetFullPath(std::string & cmd)
{
    // Get the environment variable PATH
    std::string       env_path = GetEnvironmentVariable("PATH");
    std::stringstream ss(env_path);
    std::string       path;

    while (std::getline(ss, path, ':')) /* Split "PATH" with ':' */
    {
        std::string full_path = path + "/" + cmd;
        if (std::filesystem::exists(full_path)) /* If the command exists. */
            return full_path;
    }

    return ""; /* The command does not exist. */
}
