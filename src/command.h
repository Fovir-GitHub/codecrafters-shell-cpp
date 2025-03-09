#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>
#include <vector>

class Command
{
private:
    std::string              command;
    std::vector<std::string> arguments;

    enum {
        ERROR,
        EXIT,
        ECHO,
        TYPE,
    };

public:
    Command(const std::string & line_command);
    Command();

    int getCommand();

    /**
     *@brief Execute the command.
     */
    void ExecCommand();

    /**
     *@brief The `exit` command.
     */
    void exit();

    /**
     *@brief The `echo` command.
     */
    void echo();

    void type();
};

#endif // !_COMMAND_H_