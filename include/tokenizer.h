#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_SEMICOLON,
    TOKEN_INPUT_REDIRECT,
    TOKEN_OUTPUT_REDIRECT,
    TOKEN_APPEND_REDIRECT,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_EOF
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

Token* tokenize(char *line, int *token_count);
void free_tokens(Token *tokens, int count);

#endif