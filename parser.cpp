#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

#include "parser.h"
#include "errors.h"

/*
    PARSER PATTERNS
*/

class ParserPatterns
{
    private:
        std::map<std::string, std::vector<std::string>> patterns;

    public:
        ParserPatterns();
        auto getPatterns() { return patterns; };
};

ParserPatterns::ParserPatterns()
{
    patterns["variable_decl"] = {"keyword var let", "datatype", "identifier", "literal"};
    patterns["for_loop"] = {"keyword for", "statement", "statement", "code block"},
    patterns["code block"]  = {"all"};
}

ParserPatterns parserPatterns;

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


std::vector<std::string> split(std::string text);
std::vector<std::string> slice(std::vector<std::string>& arr, int x, int y);

void Parser::parse()
{
    // std::vector<Node *> baseNodes = {main};
    // int depth = 0;
    // Node *node;

    std::vector<std::vector<std::string>> availablePatterns; // A list of string lists
    std::vector<std::vector<std::string>> workingPatterns;
    int patternDepth = 0;

    for (auto i = tokens.begin(); i != tokens.end(); ++i)
    {
        Token token = *i;
        // Node *baseNode = baseNodes[depth];

        if (token.getType() == TokenType::endcommand)
        {
            availablePatterns.clear();
            patternDepth = 0;
        }
        // Available patterns reset at the beginning of the program or the end of command
        else if (availablePatterns.size() == 0)
        {
            for (auto & p : parserPatterns.getPatterns())
            {
                // Get the second part of the dictionary item: PTTS
                auto ptts = p.second;
                // Parse the FIRST second part of the dictionary item: PTTSKEWY
                auto pttskewy = split(ptts[0]);
                // If the first item of PTTSKEWY matches the token
                if (pttskewy[0] == TokenList::tokentype_true_string(token.getType()))
                {
                    // If there are params to be parsed
                    if (pttskewy.size() > 1)
                    {
                        // Is token.getWord() one of the params? (Don't include first item)
                        if (std::find(std::begin(pttskewy) + 1, std::end(pttskewy), token.getWord()) != std::end(pttskewy))
                        {
                            // If so, then it's a valid pattern! :D
                            availablePatterns.push_back(ptts);
                            std::cout << "DEBUG ACCEPTED PATTERN: " << p.first << std::endl;
                            patternDepth++;
                        }
                    }
                    else
                    {
                        // If no params, then the token matches the pattern!
                        availablePatterns.push_back(ptts);
                        patternDepth++;
                    }
                }
            }
        }
        else
        {
            // In middle of a pattern, implement patternDepth
            int i = 0; // Iterator for removing patterns
            for (auto ptts : availablePatterns)
            {
                std::cout << "PATTERN TOKEN MATCHED: " << ptts[patternDepth] << std::endl;
                auto pttskewy = split(ptts[patternDepth]); // Pretty much only change :/
                if (pttskewy[0] == TokenList::tokentype_true_string(token.getType()))
                {
                    if (pttskewy.size() > 1)
                    {
                        if (std::find(std::begin(pttskewy) + 1, std::end(pttskewy), token.getWord()) != std::end(pttskewy))
                        {
                            // We keep this pattern!
                        }
                        else
                        {
                            // Remove this pattern
                            availablePatterns.erase(std::begin(availablePatterns) + i);
                        }
                    }
                }
                else
                {
                    // We keep this pattern!
                }

                i++;
                patternDepth++;
            }
        }

        // if (token.getType() == TokenType::keyword) // KEYWORD
        // {
        //     node = make_node(token, baseNode);
        //     if (baseNode->token.getType() == TokenType::none || 
        //         baseNode->token.getType() == TokenType::openblock)
        //     {
        //         updepth(&baseNodes, node, &depth);
        //     }
        // }
        // else if (token.getType() == TokenType::datatype) // DATATYPE
        // {
        //     node = make_node(token, baseNode);
        // }
        // else if (token.getType() == TokenType::identifier) // IDENTIFIER
        // {
        //     node = make_node(token, baseNode);
        //     // If identfier node is not part of another command, set it as basenode
        //     if (baseNode->token.getType() == TokenType::none || 
        //         baseNode->token.getType() == TokenType::openblock)
        //     {
        //         updepth(&baseNodes, node, &depth);
        //     }
        // }
        // else if (token.getType() == TokenType::literal) // LITERAL
        // {
        //     node = make_node(token, baseNode);
        // }
        // else if (token.getType() == TokenType::openblock || token.getType() == TokenType::openparen) // OPENBLOCK, OPENPAREN
        // {
        //     node = make_node(token, baseNode);
        //     updepth(&baseNodes, node, &depth);
        // }
        // else if (token.getType() == TokenType::closeblock || token.getType() == TokenType::closeparen || token.getType() == TokenType::endcommand) // ENDCOMMAND AND CLOSEBLOCK
        // {
        //     if (depth == 0) errorHandler.invokeError(ErrorType::inadequateDepth, token.getLine());
        //     baseNodes.pop_back();
        //     depth--;
        // }
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

        std::cout << TokenList::tokentype_debug_string(child->token.getType()) << std::string(indent, '\t'); // Display the node's token type and indentation
        std::cout << "\t-\t" << child->token.getWord() << std::endl; // Dash and node token word

        _debug(i, indent+1);
    }
}

// Split string by spaces
std::vector<std::string> split(std::string text)
{
    std::istringstream ss(text);
    std::string word = "";
    std::vector<std::string> words;
    while (ss >> word)
    {
        words.push_back(word);
    }
    return words;
}

// Get slice of vector
std::vector<std::string> slice(std::vector<std::string>& arr, int x, int y)
{
    // Get start and end iterators
    auto start = arr.begin() + x;
    auto end = arr.begin() + y + 1;
    // Stores the sliced vector
    std::vector<std::string> result(start, end);
    // Return result

    return result;
}