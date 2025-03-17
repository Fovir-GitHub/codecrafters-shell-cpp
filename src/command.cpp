#include "command.h"
#include <iostream>

using namespace commmands;

CommandBase::CommandBase(std::string input)
{
}

void Echo::Exec()
{
    for (const auto & arg : GetArguments()) std::cout << arg << ' ';

    return;
}
