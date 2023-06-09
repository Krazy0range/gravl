#include <iostream>

#include "parser.h"

/*
    ==============PARSER==============
*/

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;

    // Initialize the main node
    Token empty("main", TokenType::endcommand);
    main = new Node(empty);
}

void Parser::make_node(Token token, Node *parent)
{
    Node *node = new Node(token);

    parent->children.push_back(node);
}

/*
    PARSE!
*/

void Parser::parse()
{
    for (auto i = tokens.begin(); i != tokens.end(); ++i)
    {
        Token token = *i;

        // Just add all nodes to the main node for now
        make_node(token, main);
    }
}

void Parser::debug()
{
    std::cout << "NODE TREE\n";
    std::cout << main->token.getWord() << std::endl;
    _debug(main, 0);
}

void Parser::_debug(Node *node, int indent)
{
    for (auto i = node->children.begin(); i != node->children.end(); ++i)
    {
        Node *child = *i;

        std::cout << std::string(indent, '\t') << "\t-\t" << child->token.getWord() << std::endl;

        _debug(*i, indent+1);
    }
}