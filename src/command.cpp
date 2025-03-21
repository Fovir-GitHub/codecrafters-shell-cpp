#include "command.h"
#include "shell.h"
#include "tools.h"
#include <cctype>
#include <filesystem>
#include <functional>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

std::string commands::CommandBase::HandleSingleQuote(std::istringstream & iss)
{
    std::string arg("\'"); /* Initialize with single quote sign */
    char        ch;

    while (iss.get(ch))
    {
        if (ch == '\'') /* Meet another single quote sign */
        {
            // If the next character is also single quote sign, skip it
            if (iss.peek() == '\'')
                iss.get();
            else /* The close single quote sign */
            {
                arg.push_back(ch);
                break;
            }
        }
        else
            // Treat all other characters as normal characters
            arg.push_back(ch);
    }

    return arg;
}

std::string commands::CommandBase::HandleDoubleQuote(std::istringstream & iss)
{
    std::string arg("\""); /* Initialize with double quote sign */
    char        ch;
    bool        in_double_quote = true;

    while (iss.get(ch))
    {
        if (ch == '\\') /* The character is backslash */
            arg.push_back(HandleBackSlash(iss, in_double_quote));
        else if (ch == '\"') /* Meet another double quote sign */
        {
            // The next character is also double quote sign, skip it
            if (iss.peek() == '\"')
                iss.get();
            else if (std::isgraph(iss.peek()))
            {
                iss.get(ch);
                arg.push_back(ch);
                arg.erase(arg.begin());
                in_double_quote = false;
            }
            else /* Meet the close double quote sign */
            {
                arg.push_back(ch);
                break;
            }
        }
        else
            arg.push_back(ch); /* Normal characters */
    }

    return arg;
}

char commands::CommandBase::HandleBackSlash(std::istringstream & iss,
                                            bool in_double_quote)
{
    // Special characters for in double quote mode
    static const std::string SPECIAL_CHARACTERS = "\\$\"";
    char                     result             = 0;

    if (in_double_quote) /* In double quote mode */
    {
        // The next character is the special character and store it in result
        if (SPECIAL_CHARACTERS.find(result = (iss.peek())) != std::string::npos)
            // Pop up the special character from the buffer of iss
            iss.get();
        else /* If it is not special character */
        {
            iss.unget();     /* Step back to the backslash position */
            iss.get(result); /* Re-get the backslash */
        }
    }
    else                 /* Not in the double quote signs */
        iss.get(result); /* Get the next character and return */

    return result;
}

std::pair<int, std::string>
commands::CommandBase::SetArguments(const std::string & command_line)
{
    // Declare the type of quote signs
    enum QUOTE_TYPE { NONE, SINGLE, DOUBLE };

    // String to judge quote signs
    static const std::string QUOTE_SIGNS = "\'\"";

    // Use lambda expression to get the type of quote sign
    std::function<int(char)> getQuoteType = [&](char ch) {
        return ch == '\"' ? QUOTE_TYPE::DOUBLE : QUOTE_TYPE::SINGLE;
    };

    std::istringstream iss(command_line);
    std::string        arg; /* Store the current arguments */
    char               ch;
    int                quote_type = QUOTE_TYPE::NONE;

    // Ignore the invisible characters at the beginning of the buffer
    while (iss.get(ch) && !std::isgraph(ch));

    // Step back to the first visible character
    iss.unget();
    arguments.clear(); /* Reset the list of arguments */

    while (iss.get(ch))
    {
        if (!std::isgraph(ch)) /* If the character is invisible */
        {
            if (!arg.empty())             /* If the arg is not empty */
                arguments.push_back(arg); /* Add the argument */
            arg.clear();                  /* Reset the argument */
            continue;                     /* Skip */
        }

        // The current character is quote sign
        if (QUOTE_SIGNS.find(ch) != std::string::npos)
            arguments.push_back(getQuoteType(ch) == QUOTE_TYPE::SINGLE
                                    ? HandleSingleQuote(iss)
                                    : HandleDoubleQuote(iss));
        else if (ch == '\\') /* The character is backslash */
            arg.push_back(HandleBackSlash(iss, false));
        else /* Common characters */
            arg.push_back(ch);
    }

    // Push the last argument into the list of arguments
    if (!arg.empty())
        arguments.push_back(arg);

    // Determine the type of redirection and remove the redirect path
    const static std::string REDIRECT_SIGNS = "> 1> >> 1>> 2> 2>>";
    int                      redirect_type  = REDIRECT_TYPE::STDOUT;

    // File path to redirect
    std::string                        redirect_to("");
    std::vector<std::string>::iterator iter = arguments.begin();

    // Find the redirect sign
    for (; iter != arguments.end(); iter++)
        if (REDIRECT_SIGNS.find(*iter) != std::string::npos)
        {
            if (*iter == ">" || *iter == "1>")
                redirect_type = REDIRECT_TYPE::STDOUT_TO_FILE;
            else if (*iter == ">>" || *iter == "1>>")
                redirect_type = REDIRECT_TYPE::APPEND_STDOUT_TO_FILE;
            else if (*iter == "2>")
                redirect_type = REDIRECT_TYPE::STDERR_TO_FILE;
            else if (*iter == "2>>")
                redirect_type = REDIRECT_TYPE::APPEND_STDERR_TO_FILE;
            else
                continue;

            break;
        }

    // Set the redirect path
    if (redirect_type != REDIRECT_TYPE::STDOUT)
        redirect_to = *(iter + 1);

    // Remove the arguments after the redirect sign
    arguments.erase(iter, arguments.end());

    return {redirect_type, redirect_to};
}

void commands::Echo::Exec(std::shared_ptr<Shell> sh)
{
    // Set arguments at first
    SetArguments(sh->GetInputLine());

    // Traverse the arguments list
    for (std::string arg : GetArguments())
        std::cout << removeQuoteSigns(arg) << ' ';

    // Newline
    std::cout << '\n';

    return;
}

void commands::Exit::Exec(std::shared_ptr<Shell> sh)
{
    SetArguments(sh->GetInputLine());

    int exit_code = 0;

    // Set the exit code
    if (!GetArguments().empty())
        exit_code = std::atoi(GetArguments()[0].c_str());

    // Exit the program
    std::exit(exit_code);
}

void commands::Type::Exec(std::shared_ptr<Shell> sh)
{
    SetArguments(sh->GetInputLine());

    std::unordered_map<std::string, std::string> command_list =
        sh->GetCommandList();
    std::string cmd = GetArguments()[0];

    if (!sh->CommandExist(cmd))
        std::cout << cmd << ": not found" << '\n';
    else
        std::cout << cmd << " is "
                  << (sh->IsBuiltin(cmd) ? "a shell builtin"
                                         : command_list[cmd])
                  << '\n';

    return;
}

void commands::Pwd::Exec(std::shared_ptr<Shell> sh)
{
    SetArguments(sh->GetInputLine());

    std::cout << fs::current_path().string() << '\n';
    return;
}

void commands::Cd::Exec(std::shared_ptr<Shell> sh)
{
    SetArguments(sh->GetInputLine());

    std::string home_path = sh->GetEnvironmentVariable("HOME");
    if (GetArguments().empty())
    {
        fs::current_path(home_path);
        return;
    }

    std::string target_path        = GetArguments()[0];
    size_t      home_sign_position = 0;
    while ((home_sign_position = target_path.find("~")) != std::string::npos)
        target_path.replace(home_sign_position, 1, home_path);

    if (!fs::exists(target_path))
        std::cout << "cd: " << GetArguments()[0]
                  << ": No such file or directory\n";
    else
        fs::current_path(target_path);

    return;
}
