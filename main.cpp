#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "lexer.h"
#include "parser.h"
#include "errors.h"


void doublecheck(int argc, char *argv[]);
std::string readfile(char *argv[]);

int main(int argc, char *argv[])
{

    // Double check command line inputs
    doublecheck(argc, argv);
    
    // Open and read gravl file
    std::string fcontents;
    fcontents = readfile(argv);

    // Error Handler

    ErrorHandler errorHandler(fcontents);

    // Lexer

    LexerSettings lexerSettings;
    lexerSettings.debug_words = false;
    lexerSettings.debug_tokens = true;

    Lexer lexer(fcontents, lexerSettings, errorHandler);
    lexer.lex();

    // Parser

    auto lexerTokens = lexer.getTokens();
    ParserSettings parserSettings;
    parserSettings.debug_node_tree = false;
    Parser parser(lexerTokens, parserSettings, errorHandler);
    parser.parse();

}

void doublecheck(int argc, char *argv[])
{
    // Double check command line arguments
    if (argc == 1)
    {
        std::cout << "Please include the name of the file to be split\n";
        exit(1);
    } 
    else if (argc > 2)
    {
        std::cout << "Please only include the name of the file to be split\n";
        exit(1);
    }
}

std::string readfile(char *argv[])
{
    std::fstream file;
    file.open(argv[1], std::ios::in);

    if (file.is_open())
    {
        std::ostringstream ss;
        ss << file.rdbuf() << "\n";
        return ss.str();

        file.close();
    }
    else
    {
        std::cout << "File not found";
        exit(2);
    }

    return "";
}