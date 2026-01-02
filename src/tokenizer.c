#include "tokenizer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DELIMITERS " ;|<>()"

bool append_token(Token **tokens, Token token, int *token_count,
                  int *tokens_capacity);
void free_tokens(Token *tokens, int count);
char *parse_word(const char *line, int *current_index);

Token *tokenize(char *line, int *token_count) {
    int tokens_capacity = 5;
    Token *tokens = malloc(tokens_capacity * sizeof(Token));
    if (!tokens) {
        perror("SquirrelShell: tokenize: malloc error");
        return NULL;
    }

    *token_count = 0;

    int i = 0;
    while (line[i] != '\0') {
        char ch = line[i];

        if (isspace(ch)) {
            i++;
            continue;
        }

        Token token;
        token.value = NULL;

        switch (ch) {
        case '&':
            if (line[i + 1] == '&') {
                token.type = TOKEN_AND;
                i += 2;
            } else {
                token.type = TOKEN_BACKGROUND;
                i++;
            }
            break;

        case '|':
            if (line[i + 1] == '|') {
                token.type = TOKEN_OR;
                i += 2;
            } else {
                token.type = TOKEN_PIPE;
                i++;
            }
            break;

        case ';':
            token.type = TOKEN_SEMICOLON;
            i++;
            break;

        case '(':
            token.type = TOKEN_OPEN_PAREN;
            i++;
            break;

        case ')':
            token.type = TOKEN_CLOSE_PAREN;
            i++;
            break;

        case '>':
            if (line[i + 1] == '>') {
                token.type = TOKEN_APPEND_REDIRECT;
                i += 2;
            } else {
                token.type = TOKEN_OUTPUT_REDIRECT;
                i++;
            }
            break;

        case '<':
            token.type = TOKEN_INPUT_REDIRECT;
            i++;
            break;

        default:
            token.type = TOKEN_WORD;
            token.value = parse_word(line, &i);
            if (!token.value) {
                free_tokens(tokens, *token_count);
                return NULL;
            }
            break;
        }

        // Allocate space for additional tokens.
        if (!append_token(&tokens, token, token_count, &tokens_capacity)) {
            free_tokens(tokens, *token_count);
            return NULL;
        }
    }

    // Append EOF indicator token to mark end of token stream.
    Token eof_token;
    eof_token.type = TOKEN_EOF;
    eof_token.value = NULL;
    if (!append_token(&tokens, eof_token, token_count, &tokens_capacity)) {
        free_tokens(tokens, *token_count);
        return NULL;
    }

    return tokens;
}

bool append_token(Token **tokens, Token token, int *token_count,
                  int *tokens_capacity) {
    if (*token_count >= *tokens_capacity) {
        int new_capacity = (*tokens_capacity) * 2;
        Token *temp_tokens = realloc(*tokens, new_capacity * sizeof(Token));
        if (!temp_tokens) {
            perror("SquirrelShell: append_token: realloc error");
            return false;
        }
        *tokens = temp_tokens;
        *tokens_capacity = new_capacity;
    }

    (*tokens)[*token_count] = token;
    (*token_count)++;

    return true;
}

void free_tokens(Token *tokens, int count) {
    for (int i = 0; i < count; i++) {
        free(tokens[i].value);
    }

    free(tokens);
}

char *parse_word(const char *line, int *current_index) {
    int start_index = *current_index;
    bool is_quoted = false;

    while (line[*current_index] != '\0') {
        if (line[*current_index] == '"') {
            is_quoted = !is_quoted;
        }

        if (!is_quoted && strchr(DELIMITERS, line[*current_index])) {
            break;
        }

        (*current_index)++;
    }

    size_t length = *current_index - start_index;
    char *word = strndup(&line[start_index], length);
    if (!word) {
        return NULL;
    }

    return word;
}
