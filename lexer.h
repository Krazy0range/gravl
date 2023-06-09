#ifndef lexer_h
#define lexer_h

#include <string>
#include <vector>

enum class TokenType
{
    keyword,
    identifier,
    datatype,
    literal,
    endcommand,
    none,
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

class TokenList
{
    private:
        std::vector<Token> tokens;

    public:
        static std::string tokentype_to_string(TokenType type, bool debug);
        void make_token(std::string word, TokenType type);
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
        std::vector<std::string> split(std::string text);
    
    public:
        Lexer(std::string fcontent, struct LexerSettings settings);
        ~Lexer() = default;
        std::vector<Token> getTokens();
        void lex();
};

#endif