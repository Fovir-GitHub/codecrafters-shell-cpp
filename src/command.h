#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>
#include <vector>

class Command
{
private:
    std::string              command;
    std::vector<std::string> arguments;

public:
    Command(const std::string & line_command);
    Command();

    /**
     *@brief Execute the command.
     *
     */
    void ExecCommand();

    /**
     *@brief The `exit()` command.
     *
     */
    void exit();
};

#endif // !_COMMAND_H_