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
    
    public:
        Pattern(std::string name, std::vector<std::string> reqs);
        auto getReqs() { return reqs; }
        auto getName() { return name; }
        auto getDepth() { return depth; }
        void incDepth() { depth++; }
        int depth;
};


Pattern::Pattern(std::string name, std::vector<std::string> reqs)
{
    this->name = name;
    this->reqs = reqs;
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
    // patterns.push_back(Pattern {"variable declaration", {"keyword var let", "datatype", "identifier", "literal"}});
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
    std::vector<Pattern> & workingPatterns = patternLists;
    Pattern fufilledPattern = patternLists[0];
    bool pattern_searching = true;

    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {
        Token token = *t;
        // Node *baseNode = baseNodes[depth];

        if (token.getType() == TokenType::endcommand)
        {
            // If the command has ended, but we are still in the middle of a pattern

            availablePatterns.clear();
            std::cout << "FUFILLED PATTERN: " << fufilledPattern.getName() << std::endl;
            pattern_searching = true; // Back to searching for patterns

            continue; // Skip all this pattern stuff
        }

        if (pattern_searching)
        {
            workingPatterns = patternLists;
            // pattern_searching = true;
        }
        else
        {
            workingPatterns = availablePatterns;
            // pattern_searching = false;
        }

        std::cout << "WORKING PATTERNS SIZE: " << workingPatterns.size() << std::endl;

        int i = 0; // Iterator for removing patterns
        /*
            Loop through all of the patterns and check if the tokens match
        */
        for (auto & pattern : workingPatterns)
        {
            std::cout << pattern.depth << std::endl;

            auto patternreq = pattern.getReqs()[pattern.getDepth()]; // Get the item of the pattern matching our depth
            auto patternparsed = split(patternreq); // Split it (for when there are params) also random name don't ask
            // If the first item of patternparsed matches the token, AND it's just one param OR there are multiple params and the token matches
            std::string tokentype_string = TokenList::tokentype_true_string(token.getType());

            bool patternparsed0_matches_token = patternparsed[0] == tokentype_string;
            bool just_one_param = patternparsed.size() == 1;
            bool multiple_params = patternparsed.size() > 1;
            bool token_matches_params = std::find(std::begin(patternparsed) + 1, std::end(patternparsed), token.getWord()) != std::end(patternparsed);
            if ((patternparsed0_matches_token) && (just_one_param || (multiple_params && token_matches_params)))
            {
                // IT MATCHES THE PATTERN!
                if (pattern_searching)
                    pattern.incDepth();
                else
                    availablePatterns[i].incDepth();
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
                if (pattern.getDepth() == pattern.getReqs().size()-1)
                {
                    std::cout << "\tPATTERN FINISHED: " << pattern.getName() << std::endl;
                    pattern.depth = 0; // Reset depth
                    availablePatterns.erase(std::begin(availablePatterns) + i); // Remove the pattern now that it has been finished
                    // Potentially the fufilledPattern!
                    fufilledPattern = pattern;
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

        // IF THERE ARE NO MATCHING PATTERNS

        if (workingPatterns.size() == 0)
        {
            errorHandler.invokeError(ErrorType::noMatchingPatterns, token.getLine());
        }

        // Reset pattern_searching after one go-around
        pattern_searching = false;
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