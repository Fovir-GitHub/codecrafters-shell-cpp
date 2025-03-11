#include "command.h"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stack>

namespace fs = std::filesystem;

void Command::FallBack()
{
    std::cout << command << ": command not found" << '\n';
}

Command::Command(const std::string & line_command)
{
    enum QUOTE_TYPE { SINGLE, DOUBLE, NONE };
    const std::string SPECIAL_CHARACTER_SET = "\"$\\\n";

    std::istringstream iss(line_command);
    std::string        argument("");

    arguments.clear();
    iss >> command; /* Get the command. */

    char ch         = 0;
    int  quote_type = QUOTE_TYPE::NONE;

    while (iss.get(ch))
    {
        // If the character is invisible and not in quote sign.
        if (!isgraph(ch) && quote_type == QUOTE_TYPE::NONE)
        {
            // The argument is not empty.
            if (!argument.empty())
            {
                arguments.push_back(argument);
                argument.clear();
            }
            continue; /* Skip */
        }

        // The character is the backslash and not in the quote sign.
        if (ch == '\\' && quote_type != QUOTE_TYPE::SINGLE)
        {
            // Append the next character.
            argument +=
                ((SPECIAL_CHARACTER_SET.find(iss.peek()) == std::string::npos)
                     ? ch
                     : iss.get());
            continue;
        }

        // The character is the first quote sign.
        if ((ch == '\'' || ch == '\"') && quote_type == QUOTE_TYPE::NONE)
            // Update the quote_type
            quote_type = (ch == '\'' ? QUOTE_TYPE::SINGLE : QUOTE_TYPE::DOUBLE);
        else if ((ch == '\'' && quote_type == QUOTE_TYPE::SINGLE) ||
                 (ch == '\"' && quote_type == QUOTE_TYPE::DOUBLE))
        {
            if (std::isgraph(iss.peek()))
            {
                argument += iss.get();
                quote_type = QUOTE_TYPE::NONE;
                argument.erase(argument.begin());
                continue;
            }

            // The next character is the same quote sign.
            if (iss.peek() == ch)
            {
                iss.get(); /* Ignore the next quote sign and continue. */
                continue;
            }
            else
                quote_type = QUOTE_TYPE::NONE; /* The end quote sign. */
        }

        argument += ch; /* Append the character to the argument. */
    }

    // Push the remaining argument.
    if (!argument.empty())
        arguments.push_back(argument);

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
    for (auto & s : arguments)
        std::cout << (((s.front() == '\'' && s.back() == '\'') ||
                       (s.front() == '\"' && s.back() == '\"'))
                          ? s.substr(1, s.size() - 2)
                          : s)
                  << ' ';
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
    std::cout << fs::current_path().string() << '\n';

    return;
}

void Command::cd()
{
    std::string home_path      = GetEnvironmentVariable("HOME");
    std::string home_character = "~";
    std::string temp_new_dir(arguments[0]);
    size_t      start_position = 0;

    while ((start_position = temp_new_dir.find(
                home_character, start_position)) != std::string::npos)
    {
        temp_new_dir.replace(start_position, home_character.length(),
                             home_path);
        start_position += home_path.length();
    }

    fs::path new_dir(temp_new_dir);
    if (fs::exists(new_dir) && fs::is_directory(new_dir))
        fs::current_path(new_dir);
    else
        std::cerr << "cd: " << new_dir.string() << ": No such file or directory"
                  << '\n';

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
        if (fs::exists(full_path)) /* If the command exists. */
            return full_path;
    }

    return ""; /* The command does not exist. */
}
