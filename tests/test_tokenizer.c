#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tokenizer.h"

// Prototypes.
void test_word();
void test_quoted_word();
void test_special_mixed();
void test_spaces();
void test_empty_string();
void test_all_space();
void test_improper_quotation();
void test_token(Token token, TokenType expected_type, char *expected_value);

int main()
{
    printf("==== Tokenizer ====\n");
    test_word();
    test_quoted_word();
    test_special_mixed();
    test_spaces();
    test_empty_string();
    test_all_space();
    test_improper_quotation();
}

// test 1.
void test_word()
{
    char *cmd = "ls -l";
    int token_count = 0;
    Token *tokens = tokenize(cmd, &token_count);

    assert(tokens != NULL);
    assert(token_count == 2);

    test_token(tokens[0], TOKEN_WORD, "ls");
    test_token(tokens[1], TOKEN_WORD, "-l");

    printf("test 1 passed.\n");
    free_tokens(tokens, token_count);
}

// test 2.
void test_quoted_word()
{
    char *cmd = "hi \"hello, world.\"";
    int token_count = 0;
    Token *tokens = tokenize(cmd, &token_count);

    assert(tokens != NULL);
    assert(token_count == 2);

    test_token(tokens[0], TOKEN_WORD, "hi");
    test_token(tokens[1], TOKEN_WORD, "hello, world.");

    printf("test 2 passed.\n");
    free_tokens(tokens, token_count);
}

// test 3.
void test_special_mixed()
{
    char *cmd = "cmd1 | cmd2 >> myfile.txt &";
    int token_count = 0;
    Token *tokens = tokenize(cmd, &token_count);

    assert(tokens != NULL);
    assert(token_count == 5);

    test_token(tokens[0], TOKEN_WORD, "cmd1");
    test_token(tokens[1], TOKEN_PIPE, "|");
    test_token(tokens[2], TOKEN_WORD, "cmd2");
    test_token(tokens[3], TOKEN_APPEND_REDIRECT, ">>");
    test_token(tokens[4], TOKEN_WORD, "myfile.txt");

    printf("test 3 passed.\n");
    free_tokens(tokens, token_count);
}

// test 4.
void test_spaces()
{
    char *cmd = " hello,   world.   cmd  ";
    int token_count = 0;
    Token *tokens = tokenize(cmd, &token_count);

    assert(tokens != NULL);
    assert(token_count == 3);

    test_token(tokens[0], TOKEN_WORD, "hello,");
    test_token(tokens[1], TOKEN_WORD, "world.");
    test_token(tokens[2], TOKEN_WORD, "cmd");

    printf("test 4 passed.\n");
    free_tokens(tokens, token_count);
}

// test 5.
void test_empty_string()
{
    char *cmd = "";
    int token_count = 0;
    Token *tokens = tokenize(cmd, &token_count);

    assert(tokens != NULL);
    assert(token_count == 0);

    printf("test 5 passed.\n");
    free_tokens(tokens, token_count);
}

// test 6.
void test_all_space()
{
    char *cmd = "    ";
    int token_count = 0;
    Token *tokens = tokenize(cmd, &token_count);

    assert(tokens != NULL);
    assert(token_count == 0);

    printf("test 6 passed.\n");
    free_tokens(tokens, token_count);
}

// test 7.
void test_improper_quotation()
{
    char *cmd = "hello \"world";
    int token_count = 0;
    Token *tokens = tokenize(cmd, &token_count);

    assert(tokens == NULL);

    printf("test 7 passed.\n");
}


void test_token(Token token, TokenType expected_type, char *expected_value)
{
    assert(token.type == expected_type);
    assert(strcmp(token.value, expected_value) == 0);
}


