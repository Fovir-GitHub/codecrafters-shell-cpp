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
    : redirect_type(REDIRECT_TYPE::NONE)
    , command("")
{
    /*
    The type of quote signs.
    SINGLE: '\''
    DOUBLE: '\"'
    NONE: ''
     */
    enum QUOTE_TYPE { SINGLE, DOUBLE, NONE };

    std::istringstream iss(line_command);
    std::string        argument("");
    char               ch = 0;
    int quote_type        = QUOTE_TYPE::NONE; /* Initialize with NONE. */

    arguments.clear(); /* Clear the arguments vector. */

    // Get the command.
    while (iss.get(ch))
    {
        // The character is invisible.
        if (!std::isgraph(ch))
            break;

        // The character is quote sign.
        if (ch == '\'' || ch == '\"')
        {
            // Backup the quote sign.
            char quote_sign = ch;

            // Append the new characters until meeting paird quote sign.
            while (iss.get(ch) && ch != quote_sign) command += ch;

            /*
            If the command is external command,
            then add quote signs around it.
            */
            if (command_map.find(command) == command_map.end())
            {
                command.insert(command.begin(), quote_sign);
                command += quote_sign;
            }

            break;
        }

        command += ch; /* Append the character by default. */
    }

    // Get the arguments.
    while (iss.get(ch))
    {
        /*
        If the character is invisible and not in quote sign,
        then add the argument.
        */
        if (!isgraph(ch) && quote_type == QUOTE_TYPE::NONE)
        {
            // The argument is not empty.
            if (!argument.empty())
            {
                arguments.push_back(argument);
                argument.clear(); /* Clear the string for next argument. */
            }
            continue; /* Skip */
        }

        // The character is the backslash and not in the single quote sign.
        if (ch == '\\' && quote_type != QUOTE_TYPE::SINGLE)
        {
            /*
            If it is in the double quote signs,
            check the next character of backslash sign.
            If the next character is not special character,
            then append the original character, that is backslash directly.
            Otherwise, treat the next character as the escape character
            by ignoring the backslash and append the next character directly.
            */
            argument +=
                ((quote_type == QUOTE_TYPE::DOUBLE &&
                  (SPECIAL_CHARACTER_SET.find(iss.peek())) == std::string::npos)
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
            // The next character is the same quote sign.
            if (iss.peek() == ch)
            {
                /*
                The next character is the same quote sign,
                so this is not the end of the quote.
                Ignore the two quote sign and continue.
                */
                iss.get();
                continue;
            }
            // The next character is visible.
            else if (std::isgraph(iss.peek()))
            {
                // Ignore current character and append the next character.
                argument += iss.get();
                quote_type = QUOTE_TYPE::NONE; /* Update the quote_type. */

                // The first character of current argument must be quote sign.
                argument.erase(argument.begin());
                continue; /* Skip */
            }
            else
                quote_type = QUOTE_TYPE::NONE; /* The end quote sign. */
        }

        argument += ch; /* Append the character to the argument. */
    }

    // Push the remaining argument.
    if (!argument.empty())
        arguments.push_back(argument);

    auto redirect_sign = arguments.begin();

    for (auto arg = arguments.begin(); arg != arguments.end(); arg++)
    {
        if (REDIRECT_SIGNS.find(*arg) != std::string::npos)
        {
        }

        if (*arg == ">" || *arg == "1>")
        {
            redirect_type = REDIRECT_TYPE::STDOUT;
            redirect_sign = arg;
            redirect_to   = *(arg + 1);
            break;
        }
    }

    if (redirect_type != REDIRECT_TYPE::NONE && !IsExternalCommand())
        arguments.erase(redirect_sign, arguments.end());

    arguments.push_back(""); /* Add an empty argument as the final argument. */
}

Command::Command()
{
    command = "";
    arguments.clear();
    redirect_type = REDIRECT_TYPE::NONE;
}

Command::~Command()
{
    if (redirect_type != REDIRECT_TYPE::NONE)
        std::cout.rdbuf(backup_stdout);
}

constexpr bool Command::IsExternalCommand() const
{
    return command_map.find(command) == command_map.end();
}

const int Command::GetRedirectType(const std::string & sign) const
{
    if (sign == "1>" || sign == ">")
        return REDIRECT_TYPE::STDOUT;
    else if (sign == "2>")
        return REDIRECT_TYPE::STDERR;
    else
        return REDIRECT_TYPE::NONE;
}

void Command::ExecCommand()
{
    bool is_external_command = IsExternalCommand();
    if (redirect_type != REDIRECT_TYPE::NONE)
    {
        if (is_external_command)
            redirect_type = REDIRECT_TYPE::NONE;
        else
        {
            backup_stdout = std::cout.rdbuf();
            redirect.open(redirect_to);
            std::cout.rdbuf(redirect.rdbuf());
        }
    }

    /*
    If the command is built-in command, call the corresponding function.
    Otherwise, call the external function to execute the command.
    */
    if (!is_external_command)
        command_map[command]();
    else
        command_map["external"]();

    return;
}

void Command::RunExternalCommand()
{
    // Get the real path of the external command at first.
    std::string full_path = GetFullPath(command);

    if (full_path == "") /* The command is not found. */
        FallBack();
    else
    {
        std::string full_command(command);

        // Append the arguments to the command.
        for (auto & arg : arguments) full_command += (" " + arg);
        std::system(full_command.c_str()); /* Execute the function. */
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
    // Output all arguments. And ignore the begin and end quote signs.
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

std::string Command::GetFullPath(std::string cmd)
{
    // Get the environment variable PATH
    std::string       env_path = GetEnvironmentVariable("PATH");
    std::stringstream ss(env_path);
    std::string       path;

    // The external command with single quote.
    if ((cmd.front() == '\"' && cmd.back() == '\"') ||
        (cmd.front() == '\'' && cmd.back() == '\''))
        cmd = cmd.substr(1, cmd.size() - 2);

    while (std::getline(ss, path, ':')) /* Split "PATH" with ':' */
    {
        std::string full_path = path + "/" + cmd;
        if (fs::exists(full_path)) /* If the command exists. */
            return full_path;
    }

    return ""; /* The command does not exist. */
}
