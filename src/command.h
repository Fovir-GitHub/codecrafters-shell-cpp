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

    void ExecCommand();

    void exit();
};

#endif // !_COMMAND_H_