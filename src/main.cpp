#include "shell.h"
#include <iostream>

int main()
{
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    Shell shell;

    shell.ExecuteShell();

    return 0;
}
