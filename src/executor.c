#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "command_syntax_tree.h"
#include "builtins.h"

// Built in command names
#define CHANGE_DIRECTORY "cd"
#define EXIT_SHELL "exit"

// Prototypes.
int execute_command(ASTNode *node, int last_command_status);
int execute_sequence(ASTNode *node);
int execute_logical(ASTNode *node);

// Global variables
static int last_command_status = 0;

int execute_ast(ASTNode *node)
{
    if (!node)
    {
        return 1;
    }

    switch (node->type)
    {
        case NODE_COMMAND:
            last_command_status = execute_command(node, last_command_status);
            break;

        case NODE_SEQUENCE:
            last_command_status = execute_sequence(node);
            break;

        case NODE_PIPELINE:
            break;

        case NODE_REDIRECT:
            break;
        
        case NODE_AND:
        case NODE_OR:
            last_command_status = execute_logical(node);
            break;
        
        case NODE_SUBSHELL:
            break;

        default:
            fprintf(stderr, "command not found\n");
            last_command_status = 1;
    }

    return last_command_status;
}


int execute_sequence(ASTNode *node)
{
    execute_ast(node->binary.left);
    
    return execute_ast(node->binary.right);
}


int execute_logical(ASTNode *node)
{
    int left_command_status = execute_ast(node->binary.left);

    if (node->type == NODE_AND)
    {
        if (left_command_status == 0)
        {
            return execute_ast(node->binary.right);
        }
    }
    
    else if (node->type == NODE_OR)
    {
        if (left_command_status != 0)
        {
            return execute_ast(node->binary.right);
        }
    }
    
    return left_command_status;
}


int execute_pipeline(ASTNode *node)
{
    // TODO
    return 0;
}


int execute_redirect(ASTNode *node)
{
    // TODO
    return 0;
}


int execute_command(ASTNode *node, int last_command_status)
{
    char *command = node->command.argv[0];

    if (strcmp(command, EXIT_SHELL) == 0)
    {
        exit_shell(last_command_status);
    }

    else if (strcmp(command, CHANGE_DIRECTORY) == 0)
    {
        change_directory(node->command.argv);
    }

    else
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("execute_command: fork error");
            return 1;
        }
        else if (pid == 0)
        {
            execvp(node->command.argv[0], node->command.argv);
            perror("execute_command: execvp error");
            exit(1);
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            int status_code = WEXITSTATUS(status);
            return status_code;
        }
    }

    return 0;
}