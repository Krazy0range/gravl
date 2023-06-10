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

    std::cout << "ERROR\n";
    std::cout << "Line " << line + 1 << ": "; // Line indexing doesn't start at zero, even though it is stored this way
    std::cout << error.message;
    std::cout << getFline(line);
}

Error ErrorHandler::getError(ErrorType errorType)
{
    switch (errorType)
    {
        case ErrorType::closeblock:
            return Error { "Unpaired close block found.\n", 4 };
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