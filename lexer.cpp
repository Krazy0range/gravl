#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <cctype>
#include "lexer.h"

// CONSTANTS

class Constants
{
    private:

        char OPENBLOCK = '{';
        char CLOSEBLOCK = '}';
        char OPENPAREN = '(';
        char CLOSEPAREN = ')';
        char ENDCOMMAND = ';';
        std::string SYMBOLS[5] = {
            "{",
            "}",
            "(",
            ")",
            ";"
        };
        std::string KEYWORDS[3] = {
            "var",
            "let",
            "if"
        };
        std::string DATATYPES[4] = {
            "int",
            "float",
            "string",
            "bool"
        };
    
    public:
        bool is_keyword(std::string text)
        {
            return std::find(std::begin(KEYWORDS), std::end(KEYWORDS), text) != std::end(KEYWORDS);
        }
        bool is_datatype(std::string text)
        {
            return std::find(std::begin(DATATYPES), std::end(DATATYPES), text) != std::end(DATATYPES);
        }
        bool is_symbol(std::string text)
        {
            return std::find(std::begin(SYMBOLS), std::end(SYMBOLS), text) != std::end(SYMBOLS);
        }
        bool is_symbol(char c)
        {
            std::string text(1, c);
            return std::find(std::begin(SYMBOLS), std::end(SYMBOLS), text) != std::end(SYMBOLS);
        }
        bool is_literal(std::string text)
        {
            return isdigit(text[0]) || text[0] == '-' || text[0] == '"';
        }
        char get_endcommand() { return ENDCOMMAND; };
        char get_openblock() { return OPENBLOCK; };
        char get_closeblock() { return CLOSEBLOCK; };
        char get_openparen() { return OPENPAREN; };
        char get_closeparen() { return CLOSEPAREN; };
};

Constants constants;

/*
    ==============TOKENS==============
*/

Token::Token(std::string word, TokenType type, int line)
{
    this->word = word;
    this->type = type;
    this->line = line;
}

std::string Token::getWord()
{
    return word;
}

TokenType Token::getType()
{
    return type;
}

int Token::getLine()
{
    return line;
}

/*
    TOKENLIST
*/

std::string TokenList::tokentype_to_string(TokenType type)
{
    switch (type)
    {
        case TokenType::datatype:   return "data";
        case TokenType::identifier: return "iden";
        case TokenType::keyword:    return "keyw";
        case TokenType::literal:    return "lite";
        case TokenType::endcommand: return "endc";
        case TokenType::openblock:  return "bope";
        case TokenType::closeblock: return "bclo";
        case TokenType::openparen:  return "pope";
        case TokenType::closeparen: return "pclo";
        case TokenType::none:       return "none";
        default:                    return "invalid";
    }
}

void TokenList::make_token(std::string word, TokenType type, int line)
{
    Token token(word, type, line);
    tokens.push_back(token);
}

void TokenList::debug()
{
    std::cout << "TOKENS\n";
    std::cout << "\tTokens generated: " << tokens.size() << "\n";

    for (auto i : tokens)
    {
        std::cout << "\t-\t" << tokentype_to_string(i.getType()) << '\t' << i.getWord() << '\n';
    }
}

std::vector<Token> TokenList::getTokens()
{
    return tokens;
}

/*
    ==============LEXER==============
*/

Lexer::Lexer(std::string fcontent, struct LexerSettings settings, ErrorHandler errorHandler)
{
    this->fcontent = fcontent;
    this->settings = settings;
    this->errorHandler = errorHandler;
}

std::vector<Token> Lexer::getTokens()
{
    return token_list.getTokens();
}

/*
    Tokenize the words of the gravl file
*/

void Lexer::lex()
{
    std::vector<std::string> words = split(fcontent);
    std::string word;
    std::string lastword;
    int line = 0;

    if (settings.debug_words) std::cout << "FILE WORDS\n";
    
    // Loop through all the words
    for (auto i : words)
    {
        if (settings.debug_words)
            std::cout << "\t-\t" << i << '\n';

        word = i;

        if (word == "\n")
        {
            line++;
        }
        else if (constants.is_keyword(word))
        {
            token_list.make_token(word, TokenType::keyword, line);
        }
        else if (constants.is_datatype(word))
        {
            token_list.make_token(word, TokenType::datatype, line);
        }
        else if (constants.is_literal(word))
        {
            token_list.make_token(word, TokenType::literal, line);
        }
        else if (constants.is_symbol(word))
        {
            char c = word[0];
            TokenType t;
            if (c == constants.get_openblock())
            {
                t = TokenType::openblock;
            }
            else if (c == constants.get_closeblock())
            {
                t = TokenType::closeblock;
            }
            else if (c == constants.get_openparen())
            {
                t = TokenType::openparen;
            }
            else if (c == constants.get_closeparen())
            {
                t = TokenType::closeparen;
            }
            else if (c == constants.get_endcommand())
            {
                t = TokenType::endcommand;
            }
            token_list.make_token(word, t, line);
        }
        else
        {
            token_list.make_token(word, TokenType::identifier, line);
        }

        lastword = word;
    }

    if (settings.debug_tokens)
        token_list.debug();
}

/*
    Split the contents of the gravl file into words to be tokenized
*/
std::vector<std::string> Lexer::split(std::string text)
{
    char character;
    std::string word = "";
    std::vector<std::string> words;
    bool in_string = false;
    bool in_comment = false;

    for (int i = 0; i < text.length(); i++)
    {
        character = text[i];

        if ((character == ' ' || character == '\t') && !in_string && !in_comment) // Seperate words and don't include whitespace
        {
            if (word != "")
            {
                words.push_back(word);
                word = "";
            }
        }
        else if ((character == '\n' || constants.is_symbol(character)) && !in_comment && !in_string) // Seperate words and include special characters
        {
            if (word != "")
            {
                words.push_back(word);
                word = "";
            }
            word.push_back(character);
            words.push_back(word);
            word = "";
        }
        else if (character == '\\') // Toggle commenting
        {
            in_comment = !in_comment;
            if (!in_comment)
            {
                word = "";
            }
        }
        else
        {
            word.push_back(character); // Add character to word

            if (character == '"' && !in_comment) // Toggle strings and include the quotation marks
            {
                in_string = !in_string;
            }
        }
    }

    return words;
}