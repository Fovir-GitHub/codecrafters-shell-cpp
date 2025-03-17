#include "shell.h"

Shell::Shell()
{
    // Construct command_list
    for (const auto & [key, value] : builtin_commands)
        command_list[key] = BUILTIN_COMMAND_STRING;
}

void Shell::ExecuteShell()
{
    while (true) std::cout << "$ ";
}