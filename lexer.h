#ifndef lexer_h
#define lexer_h

#include <string>
#include <list>

enum class TokenType
{
    keyword,
    identifier,
    datatype,
    symbol,
    literal
};

enum class Keyword
{
    var
};

enum class DataType
{
    integer,
    stringy,
    boolean
};

class Token
{
    private:
        std::string word;
        TokenType type;
    
    public:
        std::string getWord();
        TokenType getType();
};

class Lexer
{
    private:
        std::string fcontent;
        std::list<Token> tokens;
        std::list<std::string> split(std::string text);
    
    public:
        Lexer(std::string fcontent);
        ~Lexer() = default;
        void lex();
};

#endif