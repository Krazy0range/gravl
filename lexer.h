#ifndef lexer_h
#define lexer_h

#include <string>
#include <list>

enum class TokenType
{
    keyword,
    identifier,
    datatype,
    literal,
    endcommand,
};

class Token
{
    private:
        std::string word;
        TokenType type;
    
    public:
        Token(std::string word, TokenType type);
        std::string getWord();
        TokenType getType();
};

struct LexerSettings
{
    bool debug_words;
    bool debug_tokens;
};

class Lexer
{
    private:
        struct LexerSettings settings;
        std::string fcontent;
        std::list<Token> tokens;
        std::list<std::string> split(std::string text);
    
    public:
        Lexer(std::string fcontent, struct LexerSettings settings);
        ~Lexer() = default;
        void lex();
};

#endif