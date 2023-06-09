#include <iostream>

#include "parser.h"

/*
    ==============PARSER==============
*/

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;

    // Initialize the main node
    Token empty("main", TokenType::none);
    main = new Node(empty);
}

Node *Parser::make_node(Token token, Node *parent)
{
    Node *node = new Node(token); // Create a new node
    parent->children.push_back(node); // Add the node to its parent's children list
    return node;
}

/*
    PARSE!
*/

void Parser::parse()
{
    Node *baseNode;
    Node *node;

    for (auto i = tokens.begin(); i != tokens.end(); ++i)
    {
        Token token = *i;

        if (token.getType() == TokenType::keyword)
        {
            baseNode = make_node(token, main);
        }
        else if (token.getType() == TokenType::datatype)
        {
            node = make_node(token, baseNode);
        }
        else if (token.getType() == TokenType::identifier)
        {
            node = make_node(token, baseNode);
            if (baseNode == main)
                baseNode = node;
        }
        else if (token.getType() == TokenType::literal)
        {
            node = make_node(token, baseNode);
        }
        else if (token.getType() == TokenType::endcommand)
        {
            baseNode = main;
        }
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
    for (auto i : node->children)
    {
        Node *child = i;

        std::cout << std::string(indent, '\t') << "\t-\t" << child->token.getWord() << std::endl;

        _debug(i, indent+1);
    }
}