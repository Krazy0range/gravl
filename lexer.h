#ifndef lexer_h
#define lexer_h

#include <string>
#include <vector>

#include "errors.h"

enum class TokenType
{
    keyword,
    datatype,
    identifier,
    literal,
    endcommand,
    openblock,
    closeblock,
    openparen,
    closeparen,
    none
};

class Token
{
    private:
        std::string word;
        TokenType type;
        int line;
    
    public:
        Token(std::string word, TokenType type, int line);
        std::string getWord();
        void setWord(std::string text);
        TokenType getType();
        int getLine();
};

class TokenList
{
    private:
        std::vector<Token> tokens;

    public:
        static std::string tokentype_debug_string(TokenType type);
        static std::string tokentype_true_string(TokenType type);
        void make_token(std::string word, TokenType type, int line);
        void debug();
        std::vector<Token> getTokens();
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
        TokenList token_list;
        ErrorHandler errorHandler;
        std::vector<std::string> split(std::string text);
    
    public:
        Lexer(std::string fcontent, LexerSettings settings, ErrorHandler errorHandler);
        ~Lexer() = default;
        std::vector<Token> getTokens();
        void lex();
};

#endif