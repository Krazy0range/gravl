#include <iostream>
#include "errors.h"

/*
    ERROR HANDLER
*/

ErrorHandler::ErrorHandler(std::string fcontents)
{
    this->flines = splitlines(fcontents);
}

void ErrorHandler::invokeError(ErrorType errorType, int line)
{
    Error error = getError(errorType);

    // Color red
    std::cout << "\x1b[41mERROR on line " << line + 1 << "\x1b[0m\n"; // Line indexing doesn't start at zero, even though it is stored this way
    std::cout << error.message;
    std::cout << getFline(line);
    exit(error.exitcode);
}

Error ErrorHandler::getError(ErrorType errorType)
{
    switch (errorType)
    {
        case ErrorType::inadequateDepth:
            return Error { "Inadequate depth: unecessary closeblock or endcommand found.\n", 4 };
        case ErrorType::noMatchingPatterns:
            return Error { "No matching patterns: tokens did not match any syntax patterns.\n", 5 };
        default:
            return Error { "Invalid error", 3 };
    }
}

std::string ErrorHandler::getFline(int linenum)
{
    return flines[linenum];
}

std::vector<std::string> ErrorHandler::splitlines(std::string text)
{
    std::vector<std::string> words;
    std::string word = "";

    for (auto & c : text)
    {
        if (c == '\n')
        {
            words.push_back(word);
            word = "";
        }
        else
        {
            word.push_back(c);
        }
    }

    return words;
}