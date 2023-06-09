#ifndef parser_h
#define parser_h

#include "lexer.h"

struct Node
{
    Token token;
    std::vector<Node *> children;

    Node(Token t) : token(t) {};
};

class Parser
{
    private:
        std::vector<Token> tokens;
        Node *main;
        void _debug(Node *node, int indent);
    
    public:
        Parser(std::vector<Token> tokens);
        void make_node(Token token, Node *parent);
        void parse();
        void debug();
};

#endif