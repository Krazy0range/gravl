#ifndef parser_h
#define parser_h

#include "lexer.h"

struct Node
{
    Token token;
    std::list<Node *> children;

    Node(Token t) : token(t) {};
};

class Parser
{
    private:
        std::list<Token> tokens;
        Node *main;
        void _debug(Node *node, int indent);
    
    public:
        Parser(std::list<Token> tokens);
        void make_node(Token token, Node *parent);
        void parse();
        void debug();
};

#endif