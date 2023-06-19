#ifndef parser_h
#define parser_h

#include "lexer.h"
#include "errors.h"

struct Node
{
    Token token;
    std::vector<Node *> children;

    Node(Token t) : token(t) {};
};

struct ParserSettings
{
    bool debug_node_tree;
    bool debug_patterns;
};

class Parser
{
    private:
        std::vector<Token> tokens;
        Node *main;
        ParserSettings settings;
        ErrorHandler errorHandler;
        void _debug(Node *node, int indent);
        Node *matchPattern(std::vector<Token> tokens);
        std::vector<std::vector<Token>> splitStatements(std::vector<Token> tokens);
    
    public:
        Parser(std::vector<Token> tokens, ParserSettings settings, ErrorHandler errorHandler);
        void parse();
        void debug();
};

#endif