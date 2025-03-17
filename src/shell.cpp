#include "shell.h"
#include <sstream>

namespace fs = std::filesystem;

Shell::Shell()
{
    ConstructCommandList();

    // Construct command_list and overwrite the external command
    for (const auto & [key, value] : builtin_commands)
        command_list[key] = BUILTIN_COMMAND_STRING;
}

void Shell::ExecuteShell()
{
    while (true) std::cout << "$ ";
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
