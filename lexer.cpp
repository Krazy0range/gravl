#include <iostream>
#include <string>
#include "lexer.h"

// Token

std::string Token::getWord()
{
    return word;
}

TokenType Token::getType()
{
    return type;
}

// Lexer

Lexer::Lexer(std::string fcontent)
{
    this->fcontent = fcontent;
}

void Lexer::lex()
{
    std::list<std::string> words = split(fcontent);
    
    auto iterator = words.begin();
    for (int i = 0; i < words.size()-1; i++)
    {
        std::cout << *iterator << '\n';
        std::advance(iterator, 1);
    }
}

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