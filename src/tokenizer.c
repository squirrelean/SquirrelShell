#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tokenizer.h"

// Returns NULL on error.
Token* tokenize(char *line, int *token_count)
{
    int tokens_capacity = 5;
    Token *tokens = malloc(tokens_capacity * sizeof(Token));
    if (!tokens)
    {
        perror("malloc failure tokenizer");
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

        if (ch == '|')
        {
            token.type = TOKEN_PIPE;
            token.value = "|";
            i++;
        }
        
        else if (ch == ';')
        {
            token.type = TOKEN_SEMICOLON;
            token.value = ";";
            i++;
        }
        else if (ch == '<' || ch == '>')
        {
            char next_ch = line[i + 1];

            if (ch == '>' && next_ch == '>')
            {
                token.type = TOKEN_APPEND_REDIRECT;
                token.value = ">>";
                i+=2;
            }
            else if (ch == '>')
            {
                token.type = TOKEN_OUTPUT_REDIRECT;
                token.value = ">";
                i++;
            }
            else
            {
                token.type = TOKEN_INPUT_REDIRECT;
                token.value = "<";
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
                    // Must free word.
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
                printf("Improper quotation use\n");
                free(tokens);
                return NULL;
            }
        }
        else
        {
            // Handle regular word.
            int word_len = 0;
            int start = i;
            while (line[i] != '\0' && !isspace(line[i]) && !strchr("<>|;", line[i]))
            {
                word_len++;
                i++;
            }
            char *word = strndup(&line[start], word_len);
            if (word != NULL)
            {
                token.type = TOKEN_WORD;
                token.value = word;
                // Must free word.
            } 
            else
            {
                free_tokens(tokens, *token_count);
                return NULL;
            }
        }

        // Allocate space for additional tokens.
        if (*token_count >= tokens_capacity)
        {
            tokens_capacity *= 2;
            Token *temp_tokens = realloc(tokens, tokens_capacity * sizeof(Token));
            if (!temp_tokens)
            {
                perror("realloc failure tokenizer");
                free_tokens(tokens, *token_count);
                return NULL;
            }
            tokens = temp_tokens;
        }

        tokens[*token_count] = token;
        (*token_count)++;
    }

    return tokens;
}

void free_tokens(Token *tokens, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == TOKEN_WORD && tokens[i].value)
        {
            free(tokens[i].value);
        }
    }
    
    free(tokens);
}


