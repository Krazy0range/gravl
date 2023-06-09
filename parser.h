#ifndef parser_h
#define parser_h

#include "lexer.h"

struct Node
{
    Token token;
    std::vector<Node *> children;

    Node(Token t) : token(t) {};
};

struct ParserSettings
{
    bool debug_node_tree;
};

class Parser
{
    private:
        std::vector<Token> tokens;
        Node *main;
        ParserSettings settings;
        void _debug(Node *node, int indent);
    
    public:
        Parser(std::vector<Token> tokens, ParserSettings settings);
        Node *make_node(Token token, Node *parent);
        void parse();
        void debug();
};

#endif