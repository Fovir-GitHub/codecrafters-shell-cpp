#include "shell.h"
#include <iostream>

int main()
{
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage
    // std::cout << "$ ";

    Shell shell;

    shell.ExecuteShell();

    std::string input;
    std::getline(std::cin, input);
}
