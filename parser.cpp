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
        bool finished = false;
    
    public:
        Pattern(std::string name, std::vector<std::string> reqs);
        auto getReqs() { return reqs; }
        auto getName() { return name; }
        auto getDepth() { return depth; }
        void incDepth() { depth++; }
        auto getFinished() { return finished; }
        void finish() { finished = true; }
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
    patterns.push_back(Pattern {"undefined variable declaration", {"keyword var let", "datatype", "identifier"}});
    patterns.push_back(Pattern {"variable declaration", {"keyword var let", "datatype", "identifier", "literal"}});
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

    std::cout << settings.debug_patterns << std::endl;

    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {

        Token token = *t;
        // Node *baseNode = baseNodes[depth];

        if (token.getType() == TokenType::endcommand)
        {
            // fufilledPattern represents the final matched pattern ( or the closest one )
            // If the command has ended, but we are still in the middle of a pattern
            if (fufilledPattern.getFinished())
            {
                availablePatterns.clear(); // Clear out for next round
                pattern_searching = true; // Back to searching for patterns
                
                if (settings.debug_patterns)
                    std::cout << "FUFILLED PATTERN: " << fufilledPattern.getName() << std::endl;
            }
            else
            {
                // Pattern not completed before finish?
                errorHandler.invokeError(ErrorType::noMatchingPatterns, token.getLine());
            }

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

        int i = 0; // Iterator for removing patterns
        /*
            Loop through all of the patterns and check if the tokens match
        */
        for (auto & pattern : workingPatterns)
        {
            // TODO: THIS IS CAUSING THE BAD ALLOCATION
            if (pattern.getFinished()) // TODO <<<<<<<<<
            {
                // If the pattern has been finished, but there are still tokens lying around,
                // then we need to invalidate it

                // availablePatterns.erase(std::begin(availablePatterns) + i); // Remove the pattern now that it has been finished
                std::cout << "PATTERN INVALIDATED???????????\n";

                continue;
            }

            auto patternreq = pattern.getReqs()[pattern.getDepth()]; // Get the item of the pattern matching its depth
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
                    availablePatterns.push_back(pattern);

                    if (settings.debug_patterns)
                        std::cout << "PATTERN STARTED: " << pattern.getName() << std::endl;
                    // std::cout << "Pattern incremented depth?: " << availablePatterns[availablePatterns.size() - 1].getDepth() << std::endl;
                }

                // Debug after pattern start for style purposes
                if (settings.debug_patterns)
                    std::cout << "\tPATTERN REQ MATCHED: " << pattern.getName() << '\n';

                // If it is the end of a pattern, then we're FINISHED!
                if (pattern.getDepth() == pattern.getReqs().size()-1)
                {
                    pattern.depth = 0; // Reset depth // TODO: determine if necessary
                    pattern.finish();
                    // TODO: What if we don't remove it?
                    // TODO: We need to protect against OB indexing, and if it's OB, then don't do anything with it because it has been finished and awaiting full validation
                    // Potentially the fufilledPattern!
                    fufilledPattern = pattern;

                    if (settings.debug_patterns)
                        std::cout << "\tPATTERN FINISHED: " << pattern.getName() << std::endl;
                }
                std::cout << "survived?\n";
            }
            else
            {
                // IT DOESN'T MATCH :(
                // If not new-pattern-searching, then we need to remove this invalidated pattern
                if (!pattern_searching)
                {
                    availablePatterns.erase(std::begin(availablePatterns) + i);

                    if (settings.debug_patterns)
                        std::cout << "PATTERN REMOVED: " << pattern.getName() << std::endl;
                }
                else
                {
                    // Pattern denied in pattern_searching process
                    if (settings.debug_patterns)
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