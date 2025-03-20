#ifndef _SHELL_H_
#define _SHELL_H_

#include "command.h"
#include "trie.h"
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
    std::string       cmd                    = "";

    std::unordered_map<std::string, std::shared_ptr<commands::CommandBase>>
        builtin_commands = {
            {"echo", std::make_shared<commands::Echo>()},
            {"exit", std::make_shared<commands::Exit>()},
            {"type", std::make_shared<commands::Type>()},
            {"pwd", std::make_shared<commands::Pwd>()},
            {"cd", std::make_shared<commands::Cd>()},
    };

    Trie completion_tree;

    /**
     *@brief Set the input mode
     */
    void SetInputMode();

    /**
     *@brief Reset the input mode
     */
    void ResetInputMode();

    /**
     * @brief Get the common prefix of a group of strings
     *
     * @param possible_strings the group of strings
     * @return std::string the common prefix
     */
    std::string GetCommonPrefix(std::vector<std::string> & possible_strings);

    /**
     * @brief Handle the completion process
     * @param previous_is_tab determine whether the previous character is tab
     */
    void HandleCompletion(bool previous_is_tab);

    std::unordered_map<std::string, std::string> command_list;

public:
    Shell();
    ~Shell() {}

    const std::unordered_map<std::string, std::string> & GetCommandList() const
    {
        return command_list;
    }

    bool CommandExist(std::string cmd);
    bool IsBuiltin(std::string cmd);

    std::string GetInputLine() const { return input_line; }

    /**
     *@brief Run the shell
     */
    void ExecuteShell();

    /**
     *@brief Get the environment variable
     *
     * @param env_name The name of the environment variable
     * @return std::string The environment variable
     */
    std::string GetEnvironmentVariable(std::string env_name);

    /**
     *@brief Construct the `command_list`
     */
    void ConstructCommandList();

    /**
     * @brief Split string by given character
     *
     * @param original_string The string to split
     * @param sign The sign to split
     * @return std::vector<std::string> The result of small part string
     */
    std::vector<std::string> SplitString(std::string original_string,
                                         char        sign);

    /**
     *@brief Get input with completion from the user
     */
    void GetInput();
};

#endif // !_SHELL_H_