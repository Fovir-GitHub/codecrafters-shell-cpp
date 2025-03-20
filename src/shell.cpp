#include "shell.h"
#include "tools.h"
#include <sstream>
#include <termios.h>

namespace fs = std::filesystem;

Shell::Shell()
{
    ConstructCommandList();

    // Construct the `completion_tree`
    for (const auto & [cmd, path] : command_list) completion_tree.Insert(cmd);
}

void Shell::ExecuteShell()
{
    while (true)
    {
        std::cout << "$ ";
        GetInput(); /* Get the user's input */

        // The command exists
        if (command_list.find(cmd) != command_list.end())
        {
            // The command is built-in command
            if (command_list[cmd] == BUILTIN_COMMAND_STRING)
                builtin_commands[cmd]->Exec(std::make_shared<Shell>(*this));
            else /* Execute the original command */
                std::system((cmd + " " + input_line).c_str());
        }
        else /* The command does not exist */
            std::cout << cmd << ": command not found\n";
    }
}

std::string Shell::GetEnvironmentVariable(std::string env_name)
{
    return std::getenv(env_name.c_str());
}

void Shell::ConstructCommandList()
{
    std::vector<std::string> environment_variable_path =
        SplitString(GetEnvironmentVariable("PATH"), ':');

    for (const auto & env_path : environment_variable_path)
        if (fs::exists(env_path) && fs::is_directory(env_path))
            for (const auto & entry : fs::directory_iterator(env_path))
                command_list.insert({entry.path().filename().string(),
                                     fs::absolute(entry.path()).string()});

    // Construct command_list and overwrite the external command
    for (const auto & [key, value] : builtin_commands)
        command_list[key] = BUILTIN_COMMAND_STRING;

    return;
}

std::vector<std::string> Shell::SplitString(std::string original_string,
                                            char        sign)
{
    std::vector<std::string> result;
    std::istringstream       iss(original_string);
    std::string              token;

    while (std::getline(iss, token, sign)) result.push_back(token);

    return result;
}

void Shell::SetInputMode()
{
    termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    t.c_cc[VMIN]  = 1;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    return;
}

void Shell::ResetInputMode()
{
    termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

std::string Shell::GetCommonPrefix(std::vector<std::string> & possible_strings)
{
    // Sort the vector at first in dictionary order
    std::sort(possible_strings.begin(), possible_strings.end());

    // Get the first and last elements, which are most different
    const std::string & first = possible_strings.front();
    const std::string & last  = possible_strings.back();

    // Find the first i same character position
    size_t i = 0;
    while (i < first.length() && i < last.length() && first[i] == last[i]) i++;

    // Return the substr
    return first.substr(0, i);
}

void Shell::GetInput()
{
    SetInputMode();
    input_line.clear(); /* Clear the input line */

    char ch              = 0;
    bool previous_is_tab = false;

    while (true)
    {
        std::cin.get(ch); /* Get a character */
        if (ch == '\n')   /* If it is the newline, then break */
        {
            std::cout << '\n';
            break;
        }
        else if (ch == 127) /* The backspace */
        {
            if (!input_line.empty())
            {
                // Remove the last character
                input_line.pop_back();

                // Perform backspace
                std::cout << "\b \b";
            }
        }
        else if (ch == '\t') /* The tab to complete commands */
            HandleCompletion(previous_is_tab);
        else /* Normal characters */
        {
            input_line.push_back(ch); /* Add to the input line */
            std::cout << ch;          /* Output it */
        }

        previous_is_tab = (ch == '\t'); /* Update previous_is_tab */
    }

    ResetInputMode();

    // Get the cmd
    cmd.clear();
    std::string::iterator cmd_begin = input_line.begin(),
                          cmd_end   = input_line.end();

    // Find the first visible character
    for (; !std::isgraph(*cmd_begin); cmd_begin++);

    // If the command is in the quote signs
    if ((*cmd_begin) == '\'' || (*cmd_begin) == '\"')
    {
        // Store the quote sign
        char quote_sign = (*cmd_begin);

        // Find until meet the same quote sign
        for (cmd_end = cmd_begin + 1;
             cmd_end != input_line.end() && *cmd_end != quote_sign; cmd_end++);

        // Point cmd_end to the next position
        cmd_end++;
    }
    else
        // Find until the first invisible character
        for (cmd_end = cmd_begin;
             cmd_end != input_line.end() && std::isgraph(*cmd_end); cmd_end++);

    cmd = std::string(cmd_begin, cmd_end); /* Copy to cmd */
    cmd = removeQuoteSigns(cmd);           /* Remove the quote sign */

    // Remove the command from the input_line
    input_line.erase(input_line.begin(), cmd_end);

    return;
}

void Shell::HandleCompletion(bool previous_is_tab)
{
    std::string           command_part; /* The command in input line */
    std::string::iterator begin_command_part = input_line.begin(),
                          end_command_part   = input_line.end();
    int original_length                      = input_line.length();

    // Find the first visible character position
    for (; !std::isgraph(*begin_command_part); begin_command_part++);

    /**
     * Find the first space from the beign_command_part
     * and record it as the end of command part
     */
    for (end_command_part = begin_command_part;
         end_command_part != input_line.end() && *end_command_part != ' ';
         end_command_part++);

    // Assign the command part
    command_part = std::string(begin_command_part, end_command_part);

    // Find all possible strings
    std::vector<std::string> possible_strings =
        completion_tree.FindPossibleStringByPrefix(command_part);

    // There is no possible strings, ring the bell and exit this function
    if (possible_strings.empty())
    {
        std::cout << '\a';
        return;
    }

    // Replace the command part
    input_line.replace(begin_command_part, end_command_part,
                       GetCommonPrefix(possible_strings));

    // Clear the output and output the result of completion
    while (original_length--) std::cout << "\b \b";
    std::cout << input_line;

    /**
     * If the user inputs double tabs,
     * then output all possible commands
     */
    if (previous_is_tab)
    {
        std::cout << '\n';
        for (const std::string & possible_command : possible_strings)
            std::cout << possible_command << ' ';
        std::cout << "\n$ " << input_line;
    }

    return;
}
