#ifndef COMMAND_SYNTAX_TREE_H
#define COMMAND_SYNTAX_TREE_H

typedef enum
{
    NODE_COMMAND, // Basic commands.
    NODE_SEQUENCE, // Command chaining.
    NODE_PIPELINE, // Piped commands.
    NODE_REDIRECT,  // Redirecting destinations.
    NODE_BACKGROUND // Background process. 
} NodeType;

// Redirection type for NODE_REDIRECT.
typedef enum
{
    REDIRECT_INPUT,
    REDIRECT_OUTPUT,
    REDIRECT_APPEND
} RedirectType;

// Command node.
typedef struct
{
    char **argv;
} CommandNode;

// Binary operations.
typedef struct
{
    struct ASTNode *left;
    struct ASTNode *right;
} BinaryNode;

// Redirection node.
typedef struct
{
    struct ASTNode *child;
    char *filename;
    RedirectType redirect_type;
} RedirectNode;

// Background node.
typedef struct
{
    struct ASTNode *child;
} BackgroundNode;

// Main AST node.
typedef struct ASTNode
{
    NodeType type;

    union 
    {
        CommandNode command;
        BinaryNode binary;
        RedirectNode redirect;
        BackgroundNode background;
    };
    
} ASTNode;

#endif