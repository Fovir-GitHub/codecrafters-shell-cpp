#include "tools.h"

std::string removeQuoteSigns(std::string s)
{
    /**
     * If the length of the string is less than 2
     * or the first and last characters are not the same,
     * then return the original string
     */
    if (s.size() < 2 || s.front() != s.back())
        return s;

    /**
     * If the first character is quote sign, then return the substring
     * Otherwise, return the original string
     */
    return (s.front() == '\"' || s.front() == '\'') ? s.substr(1, s.size() - 2)
                                                    : s;
}

std::string addQuoteSigns(std::string s)
{
    // If there is space
    if (s.find(' ') != std::string::npos)
    {
        // If there is double quote inside, then add single quote
        if (s.find('\"') != std::string::npos)
            s = "\'" + s + "\'";
        else /* add double quote sign */
            s = "\"" + s + "\"";
    }

    return s;
}
