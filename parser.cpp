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
        auto getPatternLists();
};

ParserPatterns::ParserPatterns()
{
    patterns["variable_decl"] = {"keyword var let", "datatype", "identifier", "literal"};
}

auto ParserPatterns::getPatternLists()
{
    std::vector<std::vector<std::string>> patternLists;
    for (auto & i : patterns)
    {
        patternLists.push_back(i.second);
    }
    return patternLists;
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

void Parser::parse()
{
    // std::vector<Node *> baseNodes = {main};
    // int depth = 0;
    // Node *node;

    std::vector<std::vector<std::string>> patternLists = parserPatterns.getPatternLists();
    std::vector<std::vector<std::string>> availablePatterns;
    std::vector<std::vector<std::string>> workingPatterns;
    int patternDepth = 0;
    bool pattern_searching = false;

    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {
        Token token = *t;
        // Node *baseNode = baseNodes[depth];

        if (token.getType() == TokenType::endcommand)
        {
            // If the command has ended, but we are still in the middle of a pattern

            availablePatterns.clear();
            patternDepth = 0;

            continue; // Skip all this pattern stuff
        }

        if (availablePatterns.size() == 0)
        {
            workingPatterns = patternLists;
            pattern_searching = true;
        }
        else
        {
            workingPatterns = availablePatterns;
            pattern_searching = false;
        }

        int i = 0; // Iterator for removing patterns
        for (auto & ptts : workingPatterns)
        {
            auto pttsitem = ptts[patternDepth]; // Get the item of the pattern matching our depth
            auto pttskewy = split(pttsitem); // Split it (for when there are params) also random name don't ask
            // If the first item of PTTSKEWY matches the token, AND it's just one param OR there are multiple params and the token matches
            std::string tokentype_string = TokenList::tokentype_true_string(token.getType());
            bool pttskewy0_matches_token = pttskewy[0] == tokentype_string;
            bool just_one_param = pttskewy.size() == 1;
            bool multiple_params = pttskewy.size() > 1;
            bool token_matches_params = std::find(std::begin(pttskewy) + 1, std::end(pttskewy), token.getWord()) != std::end(pttskewy);
            if ((pttskewy0_matches_token) && (just_one_param || (multiple_params && token_matches_params)))
            {
                std::cout << "PATTERN MATCHED:\n\tpttsitem: " << pttsitem << "\n\ttoken: " << tokentype_string << " " << token.getWord() << std::endl;
                // IT MATCHES THE PATTERN!
                patternDepth++;
                // If it is a new pattern, add to availablePatterns
                if (pattern_searching)
                    availablePatterns.push_back(ptts);
                // If it is the end of a pattern, say so
                if (patternDepth == ptts.size())
                    std::cout << "PATTERN FINISHED\n";
            }
            else
            {
                // IT DOESN'T MATCH :(
                // If not new-pattern-searching, then we need to remove this invalidated pattern
                if (!pattern_searching)
                    availablePatterns.erase(std::begin(availablePatterns) + i);
            }

            i++;
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