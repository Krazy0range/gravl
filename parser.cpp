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
        int depth;
        bool finished = false;
        bool toBeDeleted = false;
    
    public:
        Pattern(std::string name, std::vector<std::string> reqs);
        auto getReqs() { return reqs; }
        auto getName() { return name; }
        auto getDepth() { return depth; }
        void incDepth() { depth++; }
        auto getFinished() { return finished; }
        void finish() { finished = true; }
        void markForDeletion() { toBeDeleted = true; }
        auto getToBeDeleted() { return toBeDeleted; }
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
    std::vector<Pattern> workingPatterns;
    std::vector<Pattern> workingWorking;
    Pattern *fufilledPattern = NULL;
    bool pattern_searching = true;

    int token_iter = 0;

    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {

        Token token = *t; 
        // Node *baseNode = baseNodes[depth];

        if (token.getType() == TokenType::endcommand)
        {
            // fufilledPattern represents the final matched pattern ( or the closest one )
            // If the command has ended, but we are still in the middle of a pattern
            if (fufilledPattern && fufilledPattern->getFinished())
            {
                workingPatterns.clear(); // Clear out for next round
                pattern_searching = true; // Back to searching for pattern

                if (settings.debug_patterns)
                    std::cout << "FUFILLED PATTERN: " << fufilledPattern->getName() << std::endl;
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

        // Working pattern list, for appending and removing 
        // so we don't screw up the list we're iterating through
        workingWorking = workingPatterns;

        int i = 0; // Iterator for removing patterns
        /*
            Loop through all of the patterns and check if the tokens match
        */
        for (auto & pattern : workingPatterns)
        {
            // If the pattern has been completed but we're still going, then it is invalidated
            // we need to remove it and skip this loop, and check if it is the fufilledPattern
            if (pattern.getFinished())
            {
                std::cout << "PATTERN INVALIDATED: " << pattern.getName() << std::endl;
                workingWorking[i].markForDeletion();
                fufilledPattern = NULL;
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
                // Increment the workingWorking pattern's depth
                workingWorking[i].incDepth();
                
                if (pattern_searching)
                {
                    // If we are pattern searching, then append it
                    // ACTUALLY DONT! It's already here because we copied the starter patterns
                    // We only have to remove the ones that don't match
                    // workingWorking.push_back(pattern);

                    if (settings.debug_patterns)
                        std::cout << "PATTERN STARTED: " << pattern.getName() << std::endl;
                }

                // Debug after pattern start for style purposes
                if (settings.debug_patterns)
                {
                    std::cout << "\tPATTERN REQ MATCHED: " << pattern.getName() << std::endl;
                }

                // If it is the end of a pattern, then we're FINISHED!
                if (pattern.getDepth() == pattern.getReqs().size()-1)
                {
                    workingWorking[i].finish();
                    // Potentially the fufilledPattern!
                    fufilledPattern = &workingWorking[i];

                    if (settings.debug_patterns)
                        std::cout << "\tPATTERN FINISHED: " << pattern.getName() << std::endl;
                }
            }
            else
            {
                // IT DOESN'T MATCH :(
                // If not new-pattern-searching, then we need to remove this invalidated pattern
                if (!pattern_searching)
                {
                    // workingWorking.erase(std::begin(workingPatterns) + i);
                    workingWorking[i].markForDeletion();
                    // Fix potential bug of deletion screwing up iterator position and indexing?

                    if (settings.debug_patterns)
                    {
                        std::cout << "PATTERN REMOVED: " << pattern.getName() << std::endl;
                    }
                }
                else
                {
                    // Pattern denied in pattern_searching process
                    if (settings.debug_patterns)
                        std::cout << "PATTERN DENIED: " << pattern.getName() << std::endl;
                }
            }

            // DEBUG DUMP
            std::cout << "\t\t" << "req: " << patternreq << std::endl;
            std::cout << "\t\t" << "tok: " << token.getWord() << std::endl;
            std::cout << "\t\t" << "forloop iter: " << i << std::endl;
            std::cout << "\t\t" << "tokeniter: " << token_iter << std::endl;

            i++;
        }

        // Update working patterns
        workingPatterns = workingWorking;

        // Loop through workingPatterns
        // When marked pattern found
        // Delete from workingWorking and don't advance its iterator
        auto ww = std::begin(workingWorking); // workingWorking iterator
        for (auto & wp : workingPatterns) // workingPattern iterator
        {
            Pattern pattern = wp;
            if (pattern.getToBeDeleted())
                workingWorking.erase(ww);
            else
                std::advance(ww, 1);
        }

        // Update working patterns again xd
        workingPatterns = workingWorking;

        // IF THERE ARE NO MATCHING PATTERNS

        if (workingPatterns.size() == 0)
        {
            errorHandler.invokeError(ErrorType::noMatchingPatterns, token.getLine());
        }

        // Reset pattern_searching after one go-around
        pattern_searching = false;

        token_iter++;
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