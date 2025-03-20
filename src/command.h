#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#define COMMANDS_NAMESPACE_BEGIN \
    namespace commands           \
    {
#define COMMANDS_NAMESPACE_END }

class Shell;

COMMANDS_NAMESPACE_BEGIN

class CommandBase
{
private:
    std::vector<std::string> arguments;

    // Handle special characters during `SetArguments()`
    std::string HandleSingleQuote(std::istringstream & iss);
    std::string HandleDoubleQuote(std::istringstream & iss);
    char        HandleBackSlash(std::istringstream & iss, bool in_double_quote);


public:
    CommandBase() {}
    ~CommandBase() {}

    /**
     *@brief Set the arguments vector
     *
     * @param command_line the line of arguments
     */
    void SetArguments(const std::string & command_line);

    const std::vector<std::string> & GetArguments() const { return arguments; }

    virtual void Exec(std::shared_ptr<Shell>) = 0;
};

class Echo : public CommandBase
{
public:
    Echo() = default;

    void Exec(std::shared_ptr<Shell> sh) override;
};

class Exit : public CommandBase
{
public:
    Exit() = default;

    void Exec(std::shared_ptr<Shell> sh) override;
};

class Type : public CommandBase
{
public:
    Type() = default;

    void Exec(std::shared_ptr<Shell> sh) override;
};

class Pwd : public CommandBase
{
public:
    Pwd() = default;

    void Exec(std::shared_ptr<Shell> sh) override;
};

class Cd : public CommandBase
{
public:
    Cd() = default;

    void Exec(std::shared_ptr<Shell> sh) override;
};

COMMANDS_NAMESPACE_END

#endif // !_COMMAND_H_