#include "shell.h"

Shell::Shell()
{
    // Construct command_list
    for (const auto & [key, value] : builtin_commands)
        command_list[key] = "builtin";
}

void Shell::ExecuteShell()
{
    while (true) std::cout << "$ ";
}