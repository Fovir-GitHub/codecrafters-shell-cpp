#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>
#include <vector>

#define COMMANDS_NAMESPACE_BEGIN \
    namespace commmands          \
    {
#define COMMANDS_NAMESPACE_END }

COMMANDS_NAMESPACE_BEGIN

class CommandBase
{
private:
    std::vector<std::string> arguments;

public:
    CommandBase() {}
    CommandBase(std::string input);

    std::vector<std::string> & GetArguments() { return arguments; }
    virtual void               Exec() {}
};

class Echo : public CommandBase
{
public:
    Echo() {}
    void Exec();
};

COMMANDS_NAMESPACE_END

#endif // !_COMMAND_H_