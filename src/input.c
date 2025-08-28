#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "input.h"
#include <unistd.h>

// Prototypes.
void print_prompt(size_t full_command_size);
bool ensure_matching_quotes(char *line, ssize_t chars_read, int *quote_count);

char* read_line(void)
{
    size_t buffer_size = 0;
    char *line = NULL;
    ssize_t chars_read = 0;

    char *full_command = NULL;
    size_t full_command_size = 0;

    int quote_count = 0;

    while(true)
    {
        print_prompt(full_command_size);

        chars_read = getline(&line, &buffer_size, stdin);
        if (chars_read == -1)
        {
            free(line);
            free(full_command);
            return NULL;
        }

        // Strip newline.
        if (chars_read > 0 && line[chars_read - 1] == '\n')
        {
            line[chars_read - 1] = '\0';
            chars_read--;
        }

        // Check for trailing backslash.
        bool multi_line = false;
        if (chars_read > 0 && line[chars_read - 1] == '\\')
        {
            line[chars_read - 1] = '\0';
            chars_read--;
            multi_line = true;
        }

        if (ensure_matching_quotes(line, chars_read, &quote_count))
        {
            multi_line = true;
        }

        // Allocate additional space for new input.
        size_t new_size = full_command_size + chars_read + 2;
        char *temp = realloc(full_command, new_size);
        if (!temp)
        {
            perror("SquirrelShell: read_line: realloc failure");
            free(line);
            free(full_command);
            return NULL;
        }
        full_command = temp;

        memcpy(full_command + full_command_size, line, chars_read);
        full_command_size += chars_read;

        // Separate new command with a space char.
        if (multi_line)
        {
            full_command[full_command_size] = ' ';
            full_command_size++;
        }
        else
        {
            break;
        }

    }

    free(line);

    full_command[full_command_size] = '\0';
    return full_command;
}


void print_prompt(size_t full_command_size)
{
    if (!full_command_size)
    {
        printf("SquirrelShell 🐿️:: ");
    }
    else
    {
        printf(">");
    }
}


bool ensure_matching_quotes(char *line, ssize_t chars_read, int *quote_count)
{
    for (int i = 0; i < chars_read; i++)
    {
        if (line[i] == '"')
        {
            (*quote_count)++;
        }
    }

    return (*quote_count)% 2 != 0;
}