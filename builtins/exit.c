#include "allocation_resources.h"
#include "parser.h"
#include "tokenizer.h"
#include <stdlib.h>

#include <stdio.h>

void free_all_allocated_memory();

void exit_shell()
{
    free_all_allocated_memory();

    printf("SquirrelShell: Exiting shell: Last Executed Command Status: %d\n", global_command_status);

    exit(global_command_status);
}

void free_all_allocated_memory()
{
    if (global_allocated_line) {
        free(global_allocated_line);
    }

    if (global_allocated_tokens) {
        free_tokens(global_allocated_tokens, global_allocated_token_count);
    }

    if (global_allocated_ast) {
        free_ast(global_allocated_ast);
    }
}
