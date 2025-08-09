#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "command_syntax_tree.h"

// Prototypes.
ASTNode* parse_command(Token *tokens, int token_count, int *token_index);

ASTNode* parse_token(Token *tokens, int token_count)
{
    if (token_count == 0)
    {
        return NULL;
    }

    // TODO: Call the highest order parse function.
    int token_index = 0;
    ASTNode *ast = parse_command(tokens, token_count, &token_index);

    if (token_index < token_count)
    {
        // TODO: write function to free the ast.
        return NULL;
    }

    return ast;
}


ASTNode* parse_sequence()
{
    // TODO
    return NULL;
}


ASTNode* parse_pipeline()
{
    // TODO
    return NULL;
}


ASTNode* parse_redirect()
{
    // TODO
    return NULL;
}


ASTNode* parse_command(Token *tokens, int token_count, int *token_index)
{
    if (*token_index >= token_count || tokens[*token_index].type != TOKEN_WORD)
    {
        printf("Invalid command usage\n");
        return NULL;
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        printf("parse_command: node malloc failure");
        return NULL;
    }

    node->type = NODE_COMMAND;

    int word_count = 0;
    for (int i = *token_index; i < token_count && tokens[i].type == TOKEN_WORD; i++)
    {
        word_count++;
    }

    // Make space for other tokens values.
    node->command.argv = malloc((word_count + 1) * sizeof(char*));
    if (!node->command.argv)
    {
        printf("parse_command: command.argv malloc failure");
        free(node);
        return NULL;
    }

    // Copy the token values into argv array.
    for (int i = 0; i < word_count; i++)
    {
        node->command.argv[i] = strdup(tokens[*token_index + i].value);
    }
    node->command.argv[word_count] = NULL;

    *token_index += word_count;

    return node;
}