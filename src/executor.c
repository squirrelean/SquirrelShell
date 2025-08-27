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
int execute_sequence(ASTNode *node);
int execute_logical(ASTNode *node);
int execute_pipeline(ASTNode *node);
int execute_redirect(ASTNode *node);
int execute_subshell(ASTNode *node);
int execute_command(ASTNode *node, int last_command_status);

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
            last_command_status = execute_pipeline(node);
            break;

        case NODE_REDIRECT:
            last_command_status = execute_redirect(node);
            break;
        
        case NODE_AND:
        case NODE_OR:
            last_command_status = execute_logical(node);
            break;
        
        case NODE_SUBSHELL:
            last_command_status = execute_subshell(node);
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
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1)
    {
        perror("SquirrelShell: execute_pipeline: pipe error");
        return 1;
    }

    // Child process for left side of pipe.
    pid_t left_arg_pid = fork();
    if (left_arg_pid == -1)
    {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        perror("SquirrelShell: execute_pipeline: left fork error");
        return 1;
    }

    if (!left_arg_pid)
    {
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
        {
            perror("SquirrelShell: execute_pipeline: dup2 error in left child");
            exit(1);
        }

        close(pipe_fds[0]);
        close(pipe_fds[1]);

        exit(execute_ast(node->binary.left));
    }

    // Child process for right side of pipe.
    pid_t right_arg_pid = fork();
    if (right_arg_pid == -1)
    {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        perror("SquirrelShell: execute_pipeline: right fork error");
        return 1;
    }

    if (!right_arg_pid)
    {
        if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
        {
            perror("SquirrelShell: execute_pipeline: dup2 error in right child");
            exit(1);
        }

        close(pipe_fds[0]);
        close(pipe_fds[1]);

        exit(execute_ast(node->binary.right));
    }

    // Parent process. Wait for child processes to finish.
    close(pipe_fds[0]);
    close(pipe_fds[1]);

    int status;
    waitpid(left_arg_pid, NULL, 0);
    waitpid(right_arg_pid, &status, 0);

    return WEXITSTATUS(status);
}


int execute_redirect(ASTNode *node)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("SquirrelShell: execute_redirect: fork error");
        return 1;
    }

    if (!pid)
    {
        int file_descriptor;
        int file_descriptor_2;
        int open_flag = 0;
        mode_t default_perms = 0644;

        switch (node->redirect.redirect_type)
        {
            case REDIRECT_INPUT:
                open_flag = O_RDONLY;
                file_descriptor_2 = STDIN_FILENO;
                break;
            case REDIRECT_OUTPUT:
                open_flag = O_WRONLY | O_CREAT | O_TRUNC;
                file_descriptor_2 = STDOUT_FILENO;
                break;
            case REDIRECT_APPEND:
                open_flag = O_APPEND | O_WRONLY | O_CREAT;
                file_descriptor_2 = STDOUT_FILENO;
                break;
        }

        file_descriptor = open(node->redirect.filename, open_flag, default_perms);
        if (file_descriptor == -1)
        {
            perror("SquirrelShell: execute_redirect: open error");
            exit(1);
        }

        if (dup2(file_descriptor, file_descriptor_2) == -1)
        {
            perror("SquirrelShell: execute_redirect: dup2 error");
            exit(1);
        }

        if (close(file_descriptor) == -1)
        {
            perror("SquirrelShell: execute_redirect: close error");
            exit(1);
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


int execute_subshell(ASTNode *node)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("SquirrelShell: execute_subshell: fork error");
        return 1;
    }

    if (!pid)
    {
        exit(execute_ast(node->binary.left));
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("SquirrelShell: execute_subshell: waitpid error");
            return 1;
        }

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