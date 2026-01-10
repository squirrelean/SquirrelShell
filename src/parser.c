#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command_syntax_tree.h"
#include "tokenizer.h"

ASTNode *parse_sequence(Token *tokens, int token_count, int *token_index);
ASTNode *parse_pipeline(Token *tokens, int token_count, int *token_index);
ASTNode *parse_redirect(Token *tokens, int token_count, int *token_index);
ASTNode *parse_command(Token *tokens, int token_count, int *token_index);
ASTNode *parse_logical(Token *tokens, int token_count, int *token_index);
ASTNode *parse_subshell(Token *tokens, int token_count, int *token_index);
ASTNode *parse_atom(Token *tokens, int token_count, int *token_index);
void free_ast(ASTNode *node);

ASTNode *parse_token(Token *tokens, int token_count)
{
    if (token_count == 0)
        return NULL;

    int token_index = 0;
    ASTNode *ast = parse_sequence(tokens, token_count, &token_index);
    if (!ast) {
        printf("SquirrelShell: syntax error\n");
        return NULL;
    }
    if (token_index < token_count && tokens[token_index].type != TOKEN_EOF) {
        printf("Unexpected tokens after parsing index: %d count: %d\n", token_index, token_count);
        free_ast(ast);
        return NULL;
    }

    return ast;
}

ASTNode *parse_sequence(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_left = parse_logical(tokens, token_count, token_index);
    if (!node_left)
        return NULL;

    while (*token_index < token_count && tokens[*token_index].type == TOKEN_SEMICOLON) {
        (*token_index)++;

        ASTNode *node_right = parse_logical(tokens, token_count, token_index);
        if (!node_right) {
            return NULL;
        }

        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
            return NULL;

        node->type = NODE_SEQUENCE;
        node->binary.left = node_left;
        node->binary.right = node_right;

        node_left = node;
    }

    return node_left;
}

ASTNode *parse_logical(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_left = parse_pipeline(tokens, token_count, token_index);
    if (!node_left)
        return NULL;

    while (*token_index < token_count &&
           (tokens[*token_index].type == TOKEN_AND || tokens[*token_index].type == TOKEN_OR)) {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
            return NULL;

        if (tokens[*token_index].type == TOKEN_AND) {
            node->type = NODE_AND;
        } else {
            node->type = NODE_OR;
        }

        (*token_index)++;
        ASTNode *node_right = parse_pipeline(tokens, token_count, token_index);
        if (!node_right) {
            free(node);
            return NULL;
        }

        node->binary.left = node_left;
        node->binary.right = node_right;

        node_left = node;
    }
    return node_left;
}

ASTNode *parse_pipeline(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_left = parse_redirect(tokens, token_count, token_index);
    if (!node_left)
        return NULL;

    while (*token_index < token_count && tokens[*token_index].type == TOKEN_PIPE) {
        (*token_index)++;
        ASTNode *node_right = parse_redirect(tokens, token_count, token_index);
        if (!node_right) {
            return NULL;
        }

        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
            return NULL;

        node->type = NODE_PIPELINE;
        node->binary.left = node_left;
        node->binary.right = node_right;

        node_left = node;
    }
    return node_left;
}

ASTNode *parse_redirect(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node_command = parse_atom(tokens, token_count, token_index);
    if (!node_command)
        return NULL;

    ASTNode *current_node = node_command;

    while (*token_index < token_count && (tokens[*token_index].type == TOKEN_INPUT_REDIRECT ||
                                          tokens[*token_index].type == TOKEN_OUTPUT_REDIRECT ||
                                          tokens[*token_index].type == TOKEN_APPEND_REDIRECT)) {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (!node)
            return NULL;

        node->type = NODE_REDIRECT;
        node->redirect.child = current_node;

        switch (tokens[*token_index].type) {
        case TOKEN_INPUT_REDIRECT:
            node->redirect.redirect_type = REDIRECT_INPUT;
            break;

        case TOKEN_OUTPUT_REDIRECT:
            node->redirect.redirect_type = REDIRECT_OUTPUT;
            break;

        case TOKEN_APPEND_REDIRECT:
            node->redirect.redirect_type = REDIRECT_APPEND;
            break;

        default:
            return NULL;
        }

        (*token_index)++;

        if (tokens[*token_index].type != TOKEN_WORD)
            return NULL;

        node->redirect.filename = strdup(tokens[*token_index].value);

        (*token_index)++;

        current_node = node;
    }
    return current_node;
}

ASTNode *parse_atom(Token *tokens, int token_count, int *token_index)
{
    if (*token_index >= token_count)
        return NULL;

    switch (tokens[*token_index].type) {
    case TOKEN_OPEN_PAREN:
        return parse_subshell(tokens, token_count, token_index);
        break;
    case TOKEN_WORD:
        return parse_command(tokens, token_count, token_index);
        break;
    default:
        return NULL;
    }
}

ASTNode *parse_subshell(Token *tokens, int token_count, int *token_index)
{
    (*token_index)++;

    ASTNode *subshell_node = parse_sequence(tokens, token_count, token_index);
    if (!subshell_node)
        return NULL;

    if (*token_index >= token_count || tokens[*token_index].type != TOKEN_CLOSE_PAREN)
        return NULL;

    (*token_index)++;

    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;

    node->type = NODE_SUBSHELL;
    node->binary.left = subshell_node;
    node->binary.right = NULL;

    return node;
}

ASTNode *parse_command(Token *tokens, int token_count, int *token_index)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;

    node->type = NODE_COMMAND;

    int word_count = 0;
    for (int i = *token_index; i < token_count && tokens[i].type == TOKEN_WORD; i++) {
        word_count++;
    }

    node->command.argv = malloc((word_count + 1) * sizeof(char *));
    if (!node->command.argv)
        return NULL;

    for (int i = 0; i < word_count; i++) {
        node->command.argv[i] = strdup(tokens[*token_index + i].value);
    }
    node->command.argv[word_count] = NULL;

    *token_index += word_count;

    return node;
}

void free_ast(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type) {
    case NODE_COMMAND:
        for (int i = 0; node->command.argv[i]; i++) {
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
        return;
    }

    free(node);
}
