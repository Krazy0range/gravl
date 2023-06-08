#include <iostream>

#include "parser.h"

/*
    ==============PARSER==============
*/

Parser::Parser(std::list<Token> tokens)
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
    auto iterator = std::begin(tokens);
    for (int i = 0; i < tokens.size(); i++)
    {
        Token token = *iterator;

        // Just add all nodes to the main node for now
        make_node(token, main);

        std::advance(iterator, 1);
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
    auto iterator = std::begin(node->children);
    for (int i = 0; i < node->children.size(); i++)
    {
        Node *child = *iterator;

        std::cout << std::string(indent, '\t') << "\t-\t" << child->token.getWord() << std::endl;

        _debug(*iterator, indent+1);

        std::advance(iterator, 1);
    }
}