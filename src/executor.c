#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "command_syntax_tree.h"

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