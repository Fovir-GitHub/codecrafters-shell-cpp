#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class Command
{
private:
    std::string                                            command;
    std::vector<std::string>                               arguments;
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

    /**
     *@brief Execute the command.
     */
    void ExecCommand();

    /**
     *@brief Execute external command.
     *
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