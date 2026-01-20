#ifndef ALLOCATION_RESOURCES_H
#define ALLOCATION_RESOURCES_H
#include "command_syntax_tree.h"
#include "tokenizer.h"

extern char *global_allocated_line;
extern Token *global_allocated_tokens;
extern int global_allocated_token_count;
extern ASTNode *global_allocated_ast;
extern int global_command_status;

#endif
