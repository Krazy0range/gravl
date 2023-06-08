#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "lexer.h"
#include "parser.h"

std::string fcontents;

int main(int argc, char *argv[])
{
    // Double check command line arguments
    if (argc == 1)
    {
        std::cout << "Please include the name of the file to be split\n";
        return 1;
    } 
    else if (argc > 2)
    {
        std::cout << "Please only include the name of the file to be split\n";
        return 1;
    }
    
    /*
        Open and read file into fcontents
    */

    std::fstream file;
    file.open(argv[1], std::ios::in);

    if (file.is_open())
    {
        std::ostringstream ss;
        ss << file.rdbuf() << "\n";
        fcontents = ss.str();

        file.close();
    }
    else
    {
        std::cout << "File not found";
        return 2;
    }

    // Lexer

    struct LexerSettings lexerSettings;
    lexerSettings.debug_words = false;
    lexerSettings.debug_tokens = true;

    Lexer lexer(fcontents, lexerSettings);
    lexer.lex();

    Parser parser(lexer.getTokens());
    parser.parse();
    parser.debug();

}