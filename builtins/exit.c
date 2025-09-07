#include <stdlib.h>
#include "tokenizer.h"
#include "parser.h"
#include "allocation_resources.h"

#include <stdio.h>

void free_all_allocated_memory();

void exit_shell(int exit_status)
{
    free_all_allocated_memory();

    printf("SquirrelShell: Exiting shell: Last Executed Command Status: %d\n", exit_status);
    
    exit(exit_status);
}

void free_all_allocated_memory()
{
    if (global_allocated_line)
    {
        free(global_allocated_line);
    }

    if (global_allocated_tokens)
    {
        free_tokens(global_allocated_tokens, global_allocated_token_count);
    }

    if (global_allocated_ast)
    {
        free_ast(global_allocated_ast);
    }
}