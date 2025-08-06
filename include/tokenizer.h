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
    TOKEN_BACKGROUND,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_EOF,
    TOKEN_INVALID
} TokenType;

typedef struct
{
    TokenType type;
    char *value;
} Token;

#endif