#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "tokenizer.h"
#include "parser.h"
#include "builtins.h"
#include "executor.h"
#include "allocation_resources.h"

char *global_allocated_line = NULL;
Token *global_allocated_tokens = NULL;
int global_allocated_token_count = 0;
ASTNode *global_allocated_ast = NULL;

int main()
{
    do 
    {
        char *line_read = read_line();
        if (!line_read)
        {
            continue;
        }
        global_allocated_line = line_read;
        
        int token_count = 0;
        Token *tokens = tokenize(line_read, &token_count);
        if (!tokens)
        {
            free(line_read);
            continue;
        }
        global_allocated_tokens = tokens;
        global_allocated_token_count = token_count;
    
        ASTNode *ast_root_node = parse_token(tokens, token_count);
        if (!ast_root_node)
        {
            free(line_read);
            free_tokens(tokens, token_count);
            continue;
        }
        global_allocated_ast = ast_root_node;

        execute_ast(ast_root_node);
        
        // Free all allocated space.
        free(line_read);
        free_tokens(tokens, token_count);
        free_ast(ast_root_node);
    } while (true);
    

    return EXIT_SUCCESS;
}