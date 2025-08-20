#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "command_syntax_tree.h"
#include "builtins.h"

#define CHANGE_DIRECTORY "cd"

// Prototypes.
int execute_command(ASTNode *node);

void execute_ast(ASTNode *node)
{
    if (!node)
    {
        return;
    }

    switch (node->type)
    {
        case NODE_COMMAND:
            execute_command(node);
            break;

        default:
            fprintf(stderr, "command not found\n");
    }
}


int execute_command(ASTNode *node)
{
    char *command = node->command.argv[0];

    if (strcmp(command, CHANGE_DIRECTORY) == 0)
    {
        // execute change directory function.
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