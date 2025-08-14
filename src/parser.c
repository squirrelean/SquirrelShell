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
ASTNode* parse_logical(Token *tokens, int token_count, int *token_index);

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
        free_ast(ast);
        return NULL;
    }

    return ast;
}


ASTNode* parse_sequence(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_left = parse_logical(tokens, token_count, token_index);
    if (!node_left)
    {
        return NULL;
    }

    while (*token_index < token_count && 
            tokens[*token_index].type == TOKEN_SEMICOLON)
    {
        // Move past semicolon.
        (*token_index)++;

        ASTNode *node_right = parse_logical(tokens, token_count, token_index);
        if (!node_right)
        {
            return NULL;
        }

        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            fprintf(stderr, "parse_sequence: malloc failure\n");
            return NULL;
        }

        node->type = NODE_SEQUENCE;
        node->binary.left = node_left;
        node->binary.right = node_right;

        node_left = node;
    }

    return node_left;
}


ASTNode* parse_logical(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_left = parse_pipeline(tokens, token_count, token_index);
    if (!node_left)
    {
        return NULL;
    }

    while (*token_index < token_count &&
            (tokens[*token_index].type == TOKEN_AND ||
            tokens[*token_index].type == TOKEN_OR))
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            fprintf(stderr, "parse_logical: malloc failure");
            return NULL;
        }

        if (tokens[*token_index].type == TOKEN_AND)
        {
            node->type = NODE_AND;
        }
        else
        {
            node->type = NODE_OR;
        }

        (*token_index)++;
        ASTNode *node_right = parse_pipeline(tokens, token_count, token_index);
        if (!node_right)
        {
            free(node);
            return NULL;
        }

        node->binary.left = node_left;
        node->binary.right = node_right;

        node_left = node;
    }
    return node_left;
}


ASTNode* parse_pipeline(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_left = parse_redirect(tokens, token_count, token_index);
    if (!node_left)
    {
        return NULL;
    }

    while (*token_index < token_count &&
            tokens[*token_index].type == TOKEN_PIPE)
    {
        (*token_index)++;
        ASTNode *node_right = parse_redirect(tokens, token_count, token_index);
        if (!node_right)
        {
            return NULL;
        }
    
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            fprintf(stderr, "parse_pipeline: malloc failure\n");
            return NULL;
        }

        node->type = NODE_PIPELINE;
        node->binary.left = node_left;
        node->binary.right = node_right;

        node_left = node;
    }
    return node_left;
}


ASTNode* parse_redirect(Token *tokens, int token_count, int *token_index)
{
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
    if (*token_index >= token_count)
    {
        fprintf(stderr, "Token index surpasses number of tokens\n");
        return NULL;
    }

    // Subshell process below.
    if (tokens[*token_index].type == TOKEN_OPEN_PAREN)
    {
        // Move past open parenthesis.
        (*token_index)++;

        ASTNode *subshell = parse_sequence(tokens, token_count, token_index);
        if (!subshell)
        {
            fprintf(stderr, "Invalid subshell usage\n");
            return NULL;
        }

        if (*token_index >= token_count ||
            tokens[*token_index].type != TOKEN_CLOSE_PAREN)
        {
            fprintf(stderr, "Invalid parenthesis usage\n");
            return NULL;
        }

        // Move past close parenthesis.
        (*token_index)++;

        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
        {
            fprintf(stderr, "parse_command: node malloc failure");
            return NULL;
        }

        node->type = NODE_SUBSHELL;
        node->binary.left = subshell;
        node->binary.right = NULL;

        return node;
    }
    // End of subshell process.

    // Command process below.
    if (tokens[*token_index].type != TOKEN_WORD)
    {
        fprintf(stderr, "Invalid command usage\n");
        return NULL;
    }

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "parse_command: node malloc failure");
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


void free_ast(ASTNode *node)
{
    if (!node)
    {
        return;
    }

    switch (node->type)
    {
        case NODE_COMMAND:
            for (int i = 0; node->command.argv[i]; i++)
            {
                free(node->command.argv[i]);
            }
            free(node->command.argv);
            break;

        case NODE_REDIRECT:
            free_ast(node->redirect.child);
            free(node->redirect.filename);
            break;

        case NODE_PIPELINE:
        case NODE_SEQUENCE:
        case NODE_AND:
        case NODE_OR:
            free_ast(node->binary.left);
            free_ast(node->binary.right);
            break;

        case NODE_SUBSHELL:
            free_ast(node->binary.left);
            break;

        default:
    }
    
    free(node);
}