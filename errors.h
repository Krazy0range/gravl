#ifndef errors_h
#define errors_h

#include <string>
#include <vector>

enum class ErrorType
{
    closeblock
};

struct Error
{
    std::string message;
    int exitcode;
};

class ErrorHandler
{
    private:
        std::vector<std::string> flines;
        std::vector<std::string> splitlines(std::string text);
        std::string getFline(int linenum);
        Error getError(ErrorType errorType);

    public:
        ErrorHandler(std::string fcontents);
        ErrorHandler() = default;
        void invokeError(ErrorType errorType, int line);
};

#endif