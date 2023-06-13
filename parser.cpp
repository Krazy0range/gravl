#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

#include "parser.h"
#include "errors.h"

/*
    PARSER PATTERNS
*/

class Pattern
{
    private:
        std::string name;
        std::vector<std::string> reqs;
        bool finished;
    
    public:
        Pattern(std::string name, std::vector<std::string> reqs);
        auto getReqs() { return reqs; }
        auto getName() { return name; }
        auto getDepth() { return depth; }
        void incDepth() { depth++; }
        int depth;
        void finish() { finished = true; }
};


Pattern::Pattern(std::string name, std::vector<std::string> reqs)
{
    this->name = name;
    this->reqs = reqs;
    this->finished = false;
    this->depth = 0;
}

class ParserPatterns
{
    private:
        std::vector<Pattern> patterns;

    public:
        ParserPatterns();
        auto getPatterns() { return patterns; };
};

ParserPatterns::ParserPatterns()
{
    patterns.push_back(Pattern {"variable declaration", {"keyword var let", "datatype", "identifier", "literal"}});
    patterns.push_back(Pattern {"undefined variable declaration", {"keyword var let", "datatype", "identifier"}});
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

// Function definition below
std::vector<std::string> split(std::string text);

void Parser::parse()
{
    // std::vector<Node *> baseNodes = {main};
    // int depth = 0;
    // Node *node;

    std::vector<Pattern> patternLists = parserPatterns.getPatterns();
    std::vector<Pattern> availablePatterns;
    std::vector<Pattern>& workingPatterns = patternLists;
    int patternDepth = 0;
    bool pattern_searching = true;

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
        for (auto & pattern : workingPatterns)
        {
            auto patternreq = pattern.getReqs()[pattern.getDepth()]; // Get the item of the pattern matching our depth
            auto patternparsed = split(patternreq); // Split it (for when there are params) also random name don't ask
            // If the first item of patternparsed matches the token, AND it's just one param OR there are multiple params and the token matches
            std::string tokentype_string = TokenList::tokentype_true_string(token.getType());

            if (token.getWord() == "x")
            {
                std::cout << patternreq << std::endl;
            }

            bool patternparsed0_matches_token = patternparsed[0] == tokentype_string;
            bool just_one_param = patternparsed.size() == 1;
            bool multiple_params = patternparsed.size() > 1;
            bool token_matches_params = std::find(std::begin(patternparsed) + 1, std::end(patternparsed), token.getWord()) != std::end(patternparsed);
            if ((patternparsed0_matches_token) && (just_one_param || (multiple_params && token_matches_params)))
            {
                // IT MATCHES THE PATTERN!
                workingPatterns[i].incDepth();
                // pattern.depth++;
                // If it is a new pattern, add to availablePatterns
                if (pattern_searching)
                {
                    std::cout << "PATTERN STARTED: " << pattern.getName() << std::endl;
                    availablePatterns.push_back(pattern);
                    // std::cout << "Pattern incremented depth?: " << availablePatterns[availablePatterns.size() - 1].getDepth() << std::endl;
                }
                // Debug afterword for style purposes
                std::cout << "\tPATTERN REQ MATCHED: " << pattern.getName() << '\n';
                // std::cout << "\t\tpatternreq: " << patternreq << "\n\t\ttoken: " << tokentype_string << " " << token.getWord() << std::endl;
                // If it is the end of a pattern, say so
                if (patternDepth == pattern.getReqs().size())
                {
                    std::cout << "\tPATTERN FINISHED: " << pattern.getName() << std::endl;
                    pattern.finish();
                }
            }
            else
            {
                // IT DOESN'T MATCH :(
                // If not new-pattern-searching, then we need to remove this invalidated pattern
                if (!pattern_searching)
                {
                    std::cout << "PATTERN REMOVED: " << pattern.getName() << std::endl;
                    availablePatterns.erase(std::begin(availablePatterns) + i);
                }
                else
                {
                    std::cout << "PATTERN DENIED: " << pattern.getName() << std::endl;
                }
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