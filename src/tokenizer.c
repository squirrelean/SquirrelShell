#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"

// Prototypes
bool append_token(Token **tokens, Token token, int *token_count, int *tokens_capacity);
void free_tokens(Token *tokens, int count);

// Returns NULL on error.
Token* tokenize(char *line, int *token_count)
{
    int tokens_capacity = 5;
    Token *tokens = malloc(tokens_capacity * sizeof(Token));
    if (!tokens)
    {
        perror("SquirrelShell: tokenize: malloc error");
        return NULL;
    }

    *token_count = 0;

    int i = 0;
    while (line[i] != '\0')
    {
        char ch = line[i];

        if (isspace(ch))
        {
            i++;
            continue;
        }

        Token token;

        if (ch != '\0' && ch == '&' && line[i + 1] == '&')
        {
            token.type = TOKEN_AND;
            token.value = strdup("&&");
            i += 2;
        }

        else if (ch != '\0' && ch == '|' && line[i + 1] == '|')
        {
            token.type = TOKEN_OR;
            token.value = strdup("||");
            i += 2;
        }

        else if (ch == '|')
        {
            token.type = TOKEN_PIPE;
            token.value = strdup("|");
            i++;
        }
        
        else if (ch == ';')
        {
            token.type = TOKEN_SEMICOLON;
            token.value = strdup(";");
            i++;
        }

        else if (ch == '(')
        {
            token.type = TOKEN_OPEN_PAREN;
            token.value = strdup("(");
            i++;
        }

        else if (ch == ')')
        {
            token.type = TOKEN_CLOSE_PAREN;
            token.value = strdup(")");
            i++;
        }

        else if (ch == '<' || ch == '>')
        {
            char next_ch = line[i + 1];

            if (ch == '>' && next_ch == '>')
            {
                token.type = TOKEN_APPEND_REDIRECT;
                token.value = strdup(">>");
                i += 2;
            }
            else if (ch == '>')
            {
                token.type = TOKEN_OUTPUT_REDIRECT;
                token.value = strdup(">");
                i++;
            }
            else
            {
                token.type = TOKEN_INPUT_REDIRECT;
                token.value = strdup("<");
                i++;
            }
        }
        // Handle quoted words.
        else if (ch == '"')
        {
            i++;
            int start = i;
            int quote_len = 0;

            while (line[i] != '\0' && line[i] != '"')
            {
                quote_len++;
                i++;
            }

            if (line[i] == '"')
            {
                char *word = strndup(&line[start], quote_len);
                if (word != NULL)
                {
                    token.type = TOKEN_WORD;
                    token.value = word;
                }
                else
                {
                    free_tokens(tokens, *token_count);
                    return NULL;
                }
                i++;
            }
            else
            {
                free_tokens(tokens, *token_count);
                return NULL;
            }
        }
        else
        {
            // Handle regular word.
            int word_len = 0;
            int start = i;
            while (line[i] != '\0' && !isspace(line[i]) && !strchr("<>()|;", line[i]))
            {
                word_len++;
                i++;
            }
            char *word = strndup(&line[start], word_len);
            if (word != NULL)
            {
                token.type = TOKEN_WORD;
                token.value = word;
            } 
            else
            {
                free_tokens(tokens, *token_count);
                return NULL;
            }
        }

        // Allocate space for additional tokens.
        if (!append_token(&tokens, token, token_count, &tokens_capacity))
        {
            free_tokens(tokens, *token_count);
            return NULL;
        }

        // Ensure strdup did not fail.
        if (!token.value)
        {
            free_tokens(tokens, *token_count);
            return NULL;
        }
    }

    // Append EOF indicator token to mark end of stream.
    Token eof_token;
    eof_token.type = TOKEN_EOF;
    eof_token.value = NULL;
    if (!append_token(&tokens, eof_token, token_count, &tokens_capacity))
    {
        free_tokens(tokens, *token_count);
        return NULL;
    }

    return tokens;
}


// Function returns true on successful reallocation.
bool append_token(Token **tokens, Token token, int *token_count, int *tokens_capacity)
{
    if (*token_count >= *tokens_capacity)
    {
        int new_capacity = (*tokens_capacity) * 2;
        Token *temp_tokens = realloc(*tokens, new_capacity * sizeof(Token));
        if (!temp_tokens)
        {
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


void free_tokens(Token *tokens, int count)
{
    for (int i = 0; i < count; i++)
    {
        free(tokens[i].value);
    }

    free(tokens);
}
