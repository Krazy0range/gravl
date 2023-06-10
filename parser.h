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
};

class Parser
{
    private:
        std::vector<Token> tokens;
        Node *main;
        ParserSettings settings;
        ErrorHandler errorHandler;
        void _debug(Node *node, int indent);
    
    public:
        Parser(std::vector<Token> tokens, ParserSettings settings, ErrorHandler errorHandler);
        Node *make_node(Token token, Node *parent);
        void parse();
        void debug();
};

#endif