#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "command_syntax_tree.h"

// Prototypes.
ASTNode* parse_sequence(Token *tokens, int token_count, int *token_index);
ASTNode* parse_pipeline(Token *tokens, int token_count, int *token_index);
ASTNode* parse_redirect(Token *tokens, int token_count, int *token_index);
ASTNode* parse_command(Token *tokens, int token_count, int *token_index);

ASTNode* parse_token(Token *tokens, int token_count)
{
    if (token_count == 0)
    {
        return NULL;
    }

    int token_index = 0;
    ASTNode *ast = parse_sequence(tokens, token_count, &token_index);
    if (!ast)
    {
        return NULL;
    }

    if (token_index < token_count)
    {
        // TODO: write function free_ast to free the ast.
        return NULL;
    }

    return ast;
}


ASTNode* parse_sequence(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_side = parse_pipeline(tokens, token_count, token_index);
    if (!node_side)
    {
        return NULL;
    }

    if (tokens[*token_index].type != TOKEN_SEMICOLON)
    {
        return node_side;
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "parse_sequence: malloc failure\n");
        // TODO: free_ast()
        return NULL;
    }

    node->type = NODE_SEQUENCE;
    node->binary.left = node_side;

    // Move past semicolon.
    (*token_index)++;

    node->binary.right = parse_sequence(tokens, token_count, token_index);
    
    return node;
}


ASTNode* parse_pipeline(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_side = parse_redirect(tokens, token_count, token_index);
    if (!node_side)
    {
        return NULL;
    }

    if (tokens[*token_index].type != TOKEN_PIPE)
    {
        return node_side;
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "parse_pipeline: malloc failure\n");
        // TODO: free_ast()
        return NULL;
    }

    node->type = NODE_PIPELINE;
    node->binary.left = node_side;

    (*token_index)++;

    node->binary.right = parse_pipeline(tokens, token_count, token_index);

    return node;
}


ASTNode* parse_redirect(Token *tokens, int token_count, int *token_index)
{
    // TODO
    ASTNode *node_command = parse_command(tokens, token_count, token_index);
    if (!node_command)
    {
        return NULL;
    }

    ASTNode *current_node = node_command;

    while (*token_index < token_count && (
        tokens[*token_index].type == TOKEN_INPUT_REDIRECT ||
        tokens[*token_index].type == TOKEN_OUTPUT_REDIRECT ||
        tokens[*token_index].type == TOKEN_APPEND_REDIRECT))
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            fprintf(stderr, "parse_redirect: malloc failure\n");
            // TODO: free_ast()
            return NULL;
        }

        node->type = NODE_REDIRECT;
        node->redirect.child = current_node;

        switch (tokens[*token_index].type)
        {
            case TOKEN_INPUT_REDIRECT:
                node->redirect.redirect_type = REDIRECT_INPUT;
                break;
            case TOKEN_OUTPUT_REDIRECT:
                node->redirect.redirect_type = REDIRECT_OUTPUT;
                break;
            case TOKEN_APPEND_REDIRECT:
                node->redirect.redirect_type = REDIRECT_APPEND;
                break;
        }

        // Move past redirection token,
        (*token_index)++;

        if (tokens[*token_index].type != TOKEN_WORD)
        {
            fprintf(stderr, "Invalid redirection usage\n");
            return NULL;
        }
        node->redirect.filename = strdup(tokens[*token_index].value);

        // And move past filename.
        (*token_index)++;

        current_node = node;

    }
    return current_node;
}


ASTNode* parse_command(Token *tokens, int token_count, int *token_index)
{
    if (*token_index >= token_count || tokens[*token_index].type != TOKEN_WORD)
    {
        fprintf(stderr, "Invalid command usage\n");
        return NULL;
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "parse_command: node malloc failure");
        // TODO: free_ast()
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
        fprintf(stderr, "parse_command: command.argv malloc failure");
        free(node);
        // TODO: free_ast()
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


void free_ast()
{
    // TODO: Implement a function to free all allocated memory.
}