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
    const std::string BUILTIN_COMMAND_STRING = "builtin";
    std::string       input_line             = "";
    std::unordered_map<std::string, commmands::CommandBase> builtin_commands = {
        {"echo", commmands::Echo()},
    };

protected:
    std::unordered_map<std::string, std::string> command_list;

public:
    Shell();
    ~Shell() {}

    void ExecuteShell();

    std::string GetEnvironmentVariable(std::string env_name);
    void        ConstructCommandList();

    std::vector<std::string> SplitString(std::string original_string,
                                         char        sign);
};

#endif // !_SHELL_H_