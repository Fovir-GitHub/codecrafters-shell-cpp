#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "trie.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class Command
{
private:
    enum REDIRECT_TYPE { NONE, STDOUT, STDERR, APPEND_STDOUT, APPEND_STDERR };

    const std::string SPECIAL_CHARACTER_SET = "\"$\\\n";
    const std::string REDIRECT_SIGNS        = "1> > 2> >> 1>> 2>>";

    std::string                                            command;
    std::vector<std::string>                               arguments;
    int                                                    redirect_type;
    std::string                                            redirect_to;
    std::streambuf *                                       backup_redirect;
    std::ofstream                                          redirect;
    std::unordered_map<std::string, std::string>           command_list;
    Trie                                                   completion_tree;
    std::unordered_map<std::string, std::function<void()>> command_map = {
        {"exit", [this]() { exit(); }},
        {"echo", [this]() { echo(); }},
        {"type", [this]() { type(); }},
        {"pwd", [this]() { pwd(); }},
        {"cd", [this]() { cd(); }},
        {"external", [this]() { RunExternalCommand(); }},
    };

    void FallBack();

public:
    Command(const std::string & line_command);
    Command();
    ~Command();

    void InitializeCommandList();

    void InitializeCompletionTree();

    /**
     *@brief Judge whether the command is external command.
     *
     * @return true The command is external command.
     * @return false The command is builtin command.
     */
    constexpr bool IsExternalCommand() const;

    /**
     *@brief Get the redirect type.
     *
     * @param sign The sign of the redirect type.
     * @return const int The type of redirect.
     */
    const int GetRedirectType(const std::string & sign) const;

    /**
     *@brief Execute the command.
     */
    void ExecCommand();

    /**
     *@brief Execute external command.
     */
    void RunExternalCommand();

    /**
     *@brief The `exit` command.
     */
    void exit();

    /**
     *@brief The `echo` command.
     */
    void echo();

    /**
     *@brief The `type` command.
     */
    void type();

    /**
     *@brief The `pwd` command.
     */
    void pwd();

    /**
     *@brief The `cd` command.
     */
    void cd();

    /**
     *@brief Get the environment variable.
     *
     * @param env_type The type of environment variable.
     * @return std::string The environment variable
     */
    std::string GetEnvironmentVariable(const std::string & env_type);

    /**
     *@brief Get the full path of external commands.
     *
     * @param cmd
     * @return std::string The full absolute path of the command.
     */
    std::string GetFullPath(std::string cmd);
};

#endif // !_COMMAND_H_