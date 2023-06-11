#include <iostream>

#include "parser.h"
#include "errors.h"

/*
    ==============PARSER==============
*/

Parser::Parser(std::vector<Token> tokens, ParserSettings settings, ErrorHandler errorHandler)
{
    this->tokens = tokens;
    this->settings = settings;
    this->errorHandler = errorHandler;

    // Initialize the main node
    Token empty("main", TokenType::none, 0);
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

void updepth(std::vector<Node *> *baseNodes, Node *node, int *depth)
{
    baseNodes->push_back(node);
    *depth = baseNodes->size() - 1;
};

void Parser::parse()
{
    std::vector<Node *> baseNodes = {main};
    int depth = 0;
    Node *node;

    for (auto i = tokens.begin(); i != tokens.end(); ++i)
    {
        Token token = *i;
        Node *baseNode = baseNodes[depth];

        if (token.getType() == TokenType::keyword) // KEYWORD
        {
            node = make_node(token, baseNode);
            updepth(&baseNodes, node, &depth);
        }
        else if (token.getType() == TokenType::datatype) // DATATYPE
        {
            node = make_node(token, baseNode);
        }
        else if (token.getType() == TokenType::identifier) // IDENTIFIER
        {
            node = make_node(token, baseNode);
            // If identfier node is not part of another command, set it as basenode
            if (baseNode->token.getType() == TokenType::none || 
                baseNode->token.getType() == TokenType::openblock)
            {
                updepth(&baseNodes, node, &depth);
            }
        }
        else if (token.getType() == TokenType::literal) // LITERAL
        {
            node = make_node(token, baseNode);
        }
        else if (token.getType() == TokenType::openblock || token.getType() == TokenType::openparen) // OPENBLOCK, OPENPAREN
        {
            node = make_node(token, baseNode);
            updepth(&baseNodes, node, &depth);
        }
        else if (token.getType() == TokenType::closeblock || token.getType() == TokenType::closeparen || token.getType() == TokenType::endcommand) // ENDCOMMAND AND CLOSEBLOCK
        {
            if (depth == 0) errorHandler.invokeError(ErrorType::inadequateDepth, token.getLine());
            baseNodes.pop_back();
            depth--;
        }
    }

    if (settings.debug_node_tree)
        debug();
}

void Parser::debug()
{
    std::cout << "NODE TREE\n";
    std::cout << '\t' << main->token.getWord() << std::endl;
    _debug(main, 0);
}

void Parser::_debug(Node *node, int indent)
{
    for (auto i : node->children)
    {
        Node *child = i;

        std::cout << TokenList::tokentype_to_string(child->token.getType()) << std::string(indent, '\t'); // Display the node's token type and indentation
        std::cout << "\t-\t" << child->token.getWord() << std::endl; // Dash and node token word

        _debug(i, indent+1);
    }
}