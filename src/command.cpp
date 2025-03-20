#include "command.h"
#include "shell.h"
#include "tools.h"
#include <cctype>
#include <functional>
#include <iostream>
#include <sstream>

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

        // Treat all other characters as normal characters
        arg.push_back(ch);
    }

    return arg;
}

std::string commands::CommandBase::HandleDoubleQuote(std::istringstream & iss)
{
    std::string arg("\""); /* Initialize with double quote sign */
    char        ch;

    while (iss.get(ch))
    {
        if (ch == '\\') /* The character is backslash */
            arg.push_back(HandleBackSlash(iss, true));
        else if (ch == '\"') /* Meet another double quote sign */
        {
            // The next character is also double quote sign, skip it
            if (iss.peek() == '\"')
                iss.get();
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

void commands::CommandBase::SetArguments(const std::string & command_line)
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

    return;
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
