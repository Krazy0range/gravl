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
        std::string KEYWORDS[1] = {
            "var"
        };
        std::string DATATYPES[3] = {
            "int",
            "string",
            "bool"
        };
        // std::string SYMBOLS[0] = {};
    
    public:
        bool is_keyword(std::string text)
        {
            return std::find(std::begin(KEYWORDS), std::end(KEYWORDS), text) != std::end(KEYWORDS);
        }
        bool is_datatype(std::string text)
        {
            return std::find(std::begin(DATATYPES), std::end(DATATYPES), text) != std::end(DATATYPES);
        }
        bool is_endcommand(std::string text)
        {
            return text == ";";
        }
        bool is_literal(std::string text)
        {
            return isdigit(text[0]) || text[0] == '"';
        }
        // bool is_symbol(std::string text)
        // {
        //     return std::find(std::begin(SYMBOLS), std::end(SYMBOLS), text) != std::end(SYMBOLS);
        // }
};

Constants constants;

/*
    ==============TOKENS==============
*/

std::string Token::getWord()
{
    return word;
}

TokenType Token::getType()
{
    return type;
}

Token::Token(std::string word, TokenType type)
{
    this->word = word;
    this->type = type;
}

class TokenList
{
    private:
        std::list<Token> tokens;

        std::string tokentype_to_string(TokenType type, bool debug)
        {
            switch (type)
            {
                case TokenType::datatype:   return "datatype";
                case TokenType::identifier: return "identifier";
                case TokenType::keyword:    return (debug ? "keyword " : "keyword");
                case TokenType::literal:    return (debug ? "literal " : "literal");
                case TokenType::endcommand: return "endcommand";
                default:                    return "invalid";
            }
        }

    public:

        void make_token(std::string word, TokenType type)
        {
            Token token(word, type);
            tokens.push_back(token);
        }

        void debug()
        {
            std::cout << "TOKENS\n";
            std::cout << "\tTokens generated: " << tokens.size() << "\n";
            auto iterator = tokens.begin();
            for (int i = 0; i < tokens.size(); i++)
            {
                std::cout << "\t-\t" << tokentype_to_string(iterator->getType(), true) << '\t' << iterator->getWord() << '\n';
                std::advance(iterator, 1);
            }
        }
};

TokenList tokenList;

/*
    ==============LEXER==============
*/

Lexer::Lexer(std::string fcontent, struct LexerSettings settings)
{
    this->fcontent = fcontent;
    this->settings = settings;
}

/*
    Tokenize the words of the gravl file
*/

void Lexer::lex()
{
    std::list<std::string> words = split(fcontent);

    if (settings.debug_words) std::cout << "FILE WORDS\n";

    std::string word;
    std::string lastword;
    
    // Loop through all the words
    auto iterator = words.begin();
    for (int i = 0; i < words.size()-1; i++)
    {

        if (settings.debug_words)
            std::cout << "\t-\t" << *iterator << '\n';

        word = *iterator;

        if (constants.is_keyword(word))
        {
            tokenList.make_token(word, TokenType::keyword);
        }
        else if (constants.is_datatype(word))
        {
            tokenList.make_token(word, TokenType::datatype);
        }
        else if (constants.is_endcommand(word))
        {
            tokenList.make_token(word, TokenType::endcommand);
        }
        else if (constants.is_literal(word))
        {
            tokenList.make_token(word, TokenType::literal);
        }
        else
        {
            tokenList.make_token(word, TokenType::identifier);
        }

        std::advance(iterator, 1);
        lastword = word;
    }

    if (settings.debug_tokens)
        tokenList.debug();
}

/*
    Split the contents of the gravl file into words to be tokenized
*/
std::list<std::string> Lexer::split(std::string text)
{
    char character;
    std::string word = "";
    std::list<std::string> words;
    bool in_string = false;

    for (int i = 0; i < text.length(); i++)
    {
        character = text[i];

        if ((character == ' ' || character == '\n') && !in_string)
        {
            if (word != "")
            {
                words.push_back(word);
                word = "";
            }
        }
        else if (character == ';')
        {
            words.push_back(word);
            word = "";
            word.push_back(character);
            words.push_back(word);
            word = "";
        }
        else
        {
            word.push_back(character);

            if (character == '"')
            {
                in_string = !in_string;
            }
        }
    }

    words.push_back(word);

    return words;
}