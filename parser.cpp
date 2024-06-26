#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

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

Node *makeNode(Token token, Node *parent)
{
    Node *node = new Node(token); // Create a new node
    parent->children.push_back(node); // Add the node to its parent's children list

    return node;
}

void connectNodes(Node *parent, Node *child)
{
    parent->children.push_back(child);
}

/*
    PARSE!
*/

void Parser::parse()
{

    // Split the gravl code by statements
    auto statements = splitStatements(tokens);

    Node *baseNode = main;

    for (auto & statement : statements)
    {
        Node *node = matchPattern(statement);
        connectNodes(baseNode, node);
    }

    if (settings.debug_node_tree)
        debug();
}

// Split statements
std::vector<std::vector<Token>> Parser::splitStatements(std::vector<Token> tokens)
{
    std::vector<std::vector<Token>> statements;
    std::vector<Token> statement;

    for (auto & token : tokens)
    {
        statement.push_back(token);

        if (token.getType() == TokenType::endcommand)
        {
            statements.push_back(statement);
            statement.clear();
        }
    }

    return statements;
}

/*
    ==============PATTERN MATCHING CODE==============
    DIS DA BIGGEST BOI!!!!!111!!1!1!!1!!
*/


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
    AND ONTO THE REAL STUFF!!!
*/

std::vector<std::string> split(std::string text);
void buildPatternNodes(Node *head, std::vector<Token> tokens);
void matchPatternSegment(std::vector<Pattern> & workingPatterns, std::vector<Pattern> & workingWorking, Token & token, bool & pattern_searching, bool & debug_patterns, int & token_count);
void deleteMarkedPatterns(std::vector<Pattern> & workingPatterns, std::vector<Pattern> & workingWorking);
void handlePatternFinishing(std::vector<Pattern> & workingPatterns, std::vector<Token> & tokens, Token & token, Node *& head, ErrorHandler & errorHandler, bool & debug_patterns);

/*
    MATCH DOSE PATTERNS!!!1!1!11?!/1/!?/1!
    It's beautiful ;w;
*/
Node *Parser::matchPattern(std::vector<Token> tokens)
{
    static int token_count = 0;

    // Head node with empty token
    Node *head = new Node(Token("", TokenType::none, 0));

    std::vector<Pattern> patternLists = parserPatterns.getPatterns();
    std::vector<Pattern> workingPatterns;
    std::vector<Pattern> workingWorking;

    bool pattern_searching = true;

    for (auto t = tokens.begin(); t != tokens.end(); ++t)
    {

        Token token = *t;

        if (token.getType() == TokenType::endcommand)
        {
            handlePatternFinishing(workingPatterns, tokens, token, head, errorHandler, settings.debug_patterns);
            token_count++; // Keep this updated
            continue;
        }

        // Starting off with no working patterns
        if (pattern_searching)
            workingPatterns = patternLists;

        // Update workingWorking
        workingWorking = workingPatterns;

        // DIS DA BIG BOI
        matchPatternSegment(workingPatterns, workingWorking, token, pattern_searching, settings.debug_patterns, token_count);

        // Update workingPatterns
        workingPatterns = workingWorking;

        // Delete the patterns marked for deletion
        deleteMarkedPatterns(workingPatterns, workingWorking);

        // Update working patterns again xd
        workingPatterns = workingWorking;

        // If there are no matching patterns ;-;
        if (workingPatterns.size() == 0)
            errorHandler.invokeError(ErrorType::noMatchingPatterns, token.getLine());

        // Reset pattern_searching after one go-around
        pattern_searching = false;

        // Keep track of how many tokens we've gone through
        token_count++;
    }

    if (settings.debug_patterns)
        std::cout << std::endl;

    // Return the final head node with all the children attached
    return head;

}

/*
    ALL DAT CODE BEHIND THE CURTAINS
*/

void buildPatternNodes(Node *head, std::vector<Token> tokens)
{
    for (auto & token : tokens)
    {
        if (token.getType() != TokenType::endcommand)
            makeNode(token, head);
    }
}

// THE MEATY BOIIIII :heart_eyes:
void matchPatternSegment(std::vector<Pattern> & workingPatterns, std::vector<Pattern> & workingWorking, Token & token, bool & pattern_searching, bool & debug_patterns, int & token_count)
{
    int i = 0; // Iterator for removing patterns
    /*
        Loop through all of the patterns and check if the tokens match
    */
    for (auto & pattern : workingPatterns)
    {
        // If the pattern has been completed but we're still going, then it is invalidated
        if (pattern.getFinished())
        {
            if (debug_patterns)
            {
                std::cout << "PATTERN INVALIDATED: " << pattern.getName() << std::endl;
            }
            workingWorking[i].markForDeletion();
            // Don't forget to increment the iterator!!!!!!!!!
            i++;
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
            // This has caused so many problems ;-;
            workingWorking[i].incDepth();
            
            if (pattern_searching)
            {
                // If we are pattern searching, then append it
                // ACTUALLY DONT! It's already here because we copied the starter patterns
                // We only have to remove the ones that don't match
                // workingWorking.push_back(pattern);

                if (debug_patterns)
                    std::cout << "PATTERN STARTED: " << pattern.getName() << std::endl;
            }

            // Debug after pattern start for style purposes
            if (debug_patterns)
            {
                std::cout << "\tPATTERN REQ MATCHED: " << pattern.getName()  << std::endl;
            }

            // If it is the end of a pattern, then we're FINISHED!
            if (pattern.getDepth() == pattern.getReqs().size() - 1)
            {
                workingWorking[i].finish();

                if (debug_patterns)
                    std::cout << "\tPATTERN FINISHED: " << pattern.getName() << std::endl;
            }
        }
        else
        {
            // IT DOESN'T MATCH :(
            workingWorking[i].markForDeletion();
            if (!pattern_searching)
            {
                if (debug_patterns)
                    std::cout << "PATTERN REMOVED: " << pattern.getName() << std::endl;
            }
            else
            {
                if (debug_patterns)
                    std::cout << "PATTERN DENIED: " << pattern.getName() << std::endl;
            }
        }

        // DEBUG DUMP
        if (debug_patterns)
        {
            std::cout << "\x1b[2m";
            std::cout << "\t\t" << "req: " << patternreq << std::endl;
            std::cout << "\t\t" << "tok: " << token.getWord() << std::endl;
            std::cout << "\t\t" << "patterns iter: " << i << std::endl;
            std::cout << "\t\t" << "tokens iter: " << token_count << std::endl;
            std::cout << "\x1b[0m";
        }

        i++;
    }
}

void deleteMarkedPatterns(std::vector<Pattern> & workingPatterns, std::vector<Pattern> & workingWorking)
{
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
}

void handlePatternFinishing(std::vector<Pattern> & workingPatterns, std::vector<Token> & tokens, Token & token, Node *& head, ErrorHandler & errorHandler, bool & debug_patterns)
{
        bool finishedPatternFound = false;

        // Loop through the remain patterns and select the first finished one
        for (auto & pattern : workingPatterns)
        {
            if (pattern.getFinished())
            {
                head->token.setWord(pattern.getName());

                // DIS A INNER BIG BOI
                buildPatternNodes(head, tokens);

                if (debug_patterns)
                    std::cout << "\x1b[41mFUFILLED PATTERN: " << pattern.getName() << "\x1b[0m" << std::endl;
                
                finishedPatternFound = true;

                break;
            }
        }

        // Pattern not completed before finish?
        if (!finishedPatternFound)
            errorHandler.invokeError(ErrorType::noMatchingPatterns, token.getLine());
}

/*
    smol stuf i forgor ;-;
*/

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
