#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"
#include "command_syntax_tree.h"

ASTNode* parse_token(Token *tokens, int token_count);
void free_ast(ASTNode *node);

#endif