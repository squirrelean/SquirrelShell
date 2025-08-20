#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void change_directory(char **argv)
{
    if (!argv)
    {
        return;
    }

    if (argv[2] != NULL)
    {
        fprintf(stderr, "SquirrelShell: cd: too many arguments");
        return;
    }

    char *path = argv[1];

    if (!path)
    {
        path = getenv("HOME");
        if (!path)
        {
            fprintf(stderr, "SquirrelShell: cd: home enviorment variable not found");
            return;
        }
    }

    if (chdir(path) == -1)
    {
        perror("SquirrelShell: cd");
    }
}