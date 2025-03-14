#include "command.h"
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <unistd.h>

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
    InitializeCommandList();

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

    // Get the position of the sign of redirection.
    auto redirect_sign_position = arguments.end();

    /*
    If the command is external, skip this step.
    And send the original command to `std::system()` function.
    */
    for (auto arg = arguments.begin();
         !IsExternalCommand() && arg != arguments.end(); arg++)
    {
        // Find the redirect sign and the sign is avaliable.
        if (REDIRECT_SIGNS.find(*arg) != std::string::npos &&
            (redirect_type = GetRedirectType(*arg)) != REDIRECT_TYPE::NONE)
        {
            redirect_sign_position = arg;        /* Record the position. */
            redirect_to            = *(arg + 1); /* Get the redirect path. */
            break;
        }
    }

    // Remove the arguments after the redirect sign.
    arguments.erase(redirect_sign_position, arguments.end());

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
    // The command needs to be redirected.
    if (redirect_type != REDIRECT_TYPE::NONE)
    {
        // The redirect type is stdout. Restore the stdout.
        if (redirect_type == REDIRECT_TYPE::STDOUT ||
            redirect_type == REDIRECT_TYPE::APPEND_STDOUT)
            std::cout.rdbuf(backup_redirect);
        // The redirect type is stderr. Restore teh stderr.
        else
            std::cerr.rdbuf(backup_redirect);

        redirect.close(); /* Close the file. */
    }
}

void Command::InitializeCommandList()
{
    std::string       env_path = GetEnvironmentVariable("PATH");
    std::stringstream ss(env_path);
    std::string       path;

    for (auto [key, value] : command_map)
        if (key != "external")
            command_list.insert({key, "builtin"});

    while (std::getline(ss, path, ':'))
        for (const auto & entry : fs::directory_iterator(path))
            if (fs::is_regular_file(entry.path()))
                if ((fs::status(entry.path()).permissions() &
                     fs::perms::owner_exec) != fs::perms::none)
                    command_list.insert(
                        {entry.path().filename().string(),
                         GetFullPath(entry.path().filename().string())});

    return;
}

void Command::InitializeCompletionTree()
{
    for (const auto & [key, value] : command_list) completion_tree.Insert(key);

    return;
}

constexpr bool Command::IsExternalCommand() const
{
    return command_map.find(command) == command_map.end();
}

const int Command::GetRedirectType(const std::string & sign) const
{
    if (sign == "1>" || sign == ">") /* Stdout */
        return REDIRECT_TYPE::STDOUT;
    else if (sign == "2>") /* Stderr */
        return REDIRECT_TYPE::STDERR;
    else if (sign == ">>" || sign == "1>>") /* Append stdout */
        return REDIRECT_TYPE::APPEND_STDOUT;
    else if (sign == "2>>") /* Append stderr */
        return REDIRECT_TYPE::APPEND_STDERR;
    else
        return REDIRECT_TYPE::NONE;
}

void Command::ExecCommand()
{
    // Handle the redirection.
    if (redirect_type != REDIRECT_TYPE::NONE)
    {
        bool redirect_stdout = (redirect_type == REDIRECT_TYPE::STDOUT ||
                                redirect_type == REDIRECT_TYPE::APPEND_STDOUT);

        // Backup the original output stream.
        backup_redirect =
            (redirect_stdout ? std::cout.rdbuf() : std::cerr.rdbuf());

        // Open the redirect file.
        redirect.open(redirect_to,
                      (redirect_type == REDIRECT_TYPE::APPEND_STDOUT ||
                       redirect_type == REDIRECT_TYPE::APPEND_STDERR)
                          ? std::ios::app
                          : std::ios::out);

        // Redirect stdout or stderr.
        if (redirect_stdout)
            std::cout.rdbuf(redirect.rdbuf());
        else
            std::cerr.rdbuf(redirect.rdbuf());
    }

    /*
    If the command is built-in command, call the corresponding function.
    Otherwise, call the external function to execute the command.
    */
    if (!IsExternalCommand())
        command_map[command]();
    else
        command_map["external"]();

    return;
}

void Command::RunExternalCommand()
{
    if (command_list.find(command) == command_list.end())
    {
        FallBack();
        return;
    }

    std::string full_command(command);

    // Append the arguments to the command.
    for (auto & arg : arguments) full_command += (" " + arg);
    std::system(full_command.c_str()); /* Execute the function. */

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
    std::string temp_full_path =
        (command_list.find(arguments[0]) == command_list.end()
             ? ""
             : command_list[arguments[0]]);

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
