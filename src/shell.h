#ifndef _SHELL_H_
#define _SHELL_H_

#include "command.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

class Shell
{
private:
    std::string                                             input_line = "";
    std::unordered_map<std::string, commmands::CommandBase> builtin_commands = {
        {"echo", commmands::Echo()},
    };

protected:
    std::unordered_map<std::string, std::string> command_list;

public:
    Shell();
    ~Shell() {}

    void ExecuteShell();
};

#endif // !_SHELL_H_