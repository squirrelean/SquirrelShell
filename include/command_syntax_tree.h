#ifndef COMMAND_SYNTAX_TREE_H
#define COMMAND_SYNTAX_TREE_H

typedef enum
{
    NODE_COMMAND,
    NODE_SEQUENCE,
    NODE_PIPELINE,
    NODE_REDIRECT,
    NODE_AND,
    NODE_OR,
    NODE_SUBSHELL
} NodeType;

typedef enum
{
    REDIRECT_INPUT,
    REDIRECT_OUTPUT,
    REDIRECT_APPEND
} RedirectType;

typedef struct
{
    char **argv;
} CommandNode;

typedef struct
{
    struct ASTNode *left;
    struct ASTNode *right;
} BinaryNode;

typedef struct
{
    struct ASTNode *child;
    char *filename;
    RedirectType redirect_type;
} RedirectNode;

typedef struct ASTNode
{
    NodeType type;

    union 
    {
        CommandNode command;
        BinaryNode binary;
        RedirectNode redirect;
    };
    
} ASTNode;

#endif