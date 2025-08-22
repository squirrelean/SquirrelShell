#ifndef BUILTINS_H
#define BUILTINS_H
#include "command_syntax_tree.h"

void print_ast(ASTNode *ast);
void change_directory(char **path);
void exit_shell(int exit_status);

#endif