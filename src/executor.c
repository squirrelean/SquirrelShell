#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "allocation_resources.h"
#include "builtins.h"
#include "command_syntax_tree.h"

#define CHANGE_DIRECTORY "cd"
#define EXIT_SHELL "exit"
#define PRINT_AST "printast"

int execute_ast(ASTNode *node);
int execute_sequence(ASTNode *node);
int execute_logical(ASTNode *node);
int execute_pipeline(ASTNode *node);
int execute_redirect(ASTNode *node);
int execute_subshell(ASTNode *node);
int execute_command(ASTNode *node);
int execute_dispatch(ASTNode *node);

int execute_ast(ASTNode *node)
{
    if (!node) {
        global_command_status = 1;
        return global_command_status;
    }

    switch (node->type) {
    case NODE_COMMAND:
        global_command_status = execute_dispatch(node);
        break;

    case NODE_SEQUENCE:
        global_command_status = execute_sequence(node);
        break;

    case NODE_PIPELINE:
        global_command_status = execute_pipeline(node);
        break;

    case NODE_REDIRECT:
        global_command_status = execute_redirect(node);
        break;

    case NODE_AND:
    case NODE_OR:
        global_command_status = execute_logical(node);
        break;

    case NODE_SUBSHELL:
        global_command_status = execute_subshell(node);
        break;

    default:
        fprintf(stderr, "SquirrelShell: invalid node type\n");
        global_command_status = 1;
    }
    return global_command_status;
}

int execute_sequence(ASTNode *node)
{
    execute_ast(node->binary.left);

    return execute_ast(node->binary.right);
}

int execute_logical(ASTNode *node)
{
    int left_command_status = execute_ast(node->binary.left);

    if (node->type == NODE_AND) {
        if (!left_command_status) {
            return execute_ast(node->binary.right);
        }
    }

    else if (node->type == NODE_OR) {
        if (left_command_status) {
            return execute_ast(node->binary.right);
        }
    }

    return left_command_status;
}

int execute_pipeline(ASTNode *node)
{
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1)
        return 1;

    pid_t left_arg_pid = fork();
    if (left_arg_pid == -1) {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return 1;
    }

    if (!left_arg_pid) {
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
            exit(1);

        close(pipe_fds[0]);
        close(pipe_fds[1]);

        exit(execute_ast(node->binary.left));
    }

    pid_t right_arg_pid = fork();
    if (right_arg_pid == -1) {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return 1;
    }

    if (!right_arg_pid) {
        if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
            exit(1);

        close(pipe_fds[0]);
        close(pipe_fds[1]);

        exit(execute_ast(node->binary.right));
    }

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
        return 1;

    if (!pid) {
        int file_descriptor;
        int file_descriptor_2;
        int open_flag = 0;
        mode_t default_perms = 0644;

        switch (node->redirect.redirect_type) {
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
            exit(1);

        if (dup2(file_descriptor, file_descriptor_2) == -1)
            exit(1);

        if (close(file_descriptor) == -1)
            exit(1);

        exit(execute_ast(node->redirect.child));
    }

    else {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

int execute_subshell(ASTNode *node)
{
    pid_t pid = fork();
    if (pid == -1)
        return 1;

    if (!pid) {
        exit(execute_ast(node->binary.left));
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1)
            return 1;

        return WEXITSTATUS(status);
    }
}

int execute_dispatch(ASTNode *node)
{
    char *command = node->command.argv[0];

    if (strcmp(command, EXIT_SHELL) == 0)
        exit_shell();

    else if (strcmp(command, CHANGE_DIRECTORY) == 0)
        change_directory(node->command.argv);

    else if (strcmp(command, PRINT_AST) == 0)
        print_ast(node->command.argv[1]);
    else
        return execute_command(node);

    return 0;
}

int execute_command(ASTNode *node)
{
    pid_t pid = fork();
    if (pid == -1)
        return 1;
    else if (!pid) {
        execvp(node->command.argv[0], node->command.argv);
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        int status_code = WEXITSTATUS(status);
        return status_code;
    }
}
