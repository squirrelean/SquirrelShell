#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "command_syntax_tree.h"
#include "builtins.h"

// Built in command names
#define CHANGE_DIRECTORY "cd"
#define EXIT_SHELL "exit"

// Prototypes.
int execute_command(ASTNode *node, int last_command_status);
int execute_sequence(ASTNode *node);
int execute_logical(ASTNode *node);
int execute_redirect(ASTNode *node);

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
            last_command_status = execute_redirect(node);
            break;
        
        case NODE_AND:
        case NODE_OR:
            last_command_status = execute_logical(node);
            break;
        
        case NODE_SUBSHELL:
            break;

        default:
            fprintf(stderr, "SquirrelShell: command not found\n");
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
        if (!left_command_status)
        {
            return execute_ast(node->binary.right);
        }
    }
    
    else if (node->type == NODE_OR)
    {
        if (left_command_status)
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
    int file_descriptor;
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("SquirrelShell: execute_redirect: fork error");
        return 1;
    }

    if (!pid)
    {
        if (node->redirect.redirect_type == REDIRECT_INPUT)
        {
            file_descriptor = open(node->redirect.filename, O_RDONLY);
            if (file_descriptor == -1)
            {
                perror("SquirrelShell: execute_redirect: redirect_input: open error");
                return 1;
            }

            if (dup2(file_descriptor, STDIN_FILENO) == -1)
            {
                perror("SquirrelShell: execute_redirect: redirect_input: dup2 error");
                return 1;
            }
            if (close(file_descriptor) == -1)
            {
                perror("SquirrelShell: execute_redirect: redirect_input: close error");
                return 1;
            }
        }
        
        else if (node->redirect.redirect_type == REDIRECT_OUTPUT)
        {
            file_descriptor = open(node->redirect.filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (file_descriptor == -1)
            {
                perror("SquirrelShell: execute_redirect: redirect_output: open error");
                return 1;
            }
            if (dup2(file_descriptor, STDOUT_FILENO) == -1)
            {
                perror("SquirrelShell: execute_redirect: redirect_output: dup2 error");
                return 1;
            }
            if (close(file_descriptor) == -1)
            {
                perror("SquirrelShell: execute_redirect: redirect_output: close error");
                return 1;
            }
        }

        exit(execute_ast(node->redirect.child));
    }

    else
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
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
        if (pid == -1)
        {
            perror("SquirrelShell: execute_command: fork error");
            return 1;
        }
        else if (!pid)
        {
            execvp(node->command.argv[0], node->command.argv);
            perror("SquirrelShell: execute_command: execvp error");
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