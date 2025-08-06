#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "input.h"
#include <unistd.h>

char* read_line(void)
{
    size_t buffer_size = 0;
    char *line = NULL;
    ssize_t chars_read = 0;

    char *full_command = NULL;
    size_t full_command_size = 0;

    while(true)
    {
        // Print prompt.
        if (full_command_size > 0)
        {
            printf(">>> ");
        }
        else
        {
            printf("🐿️ > ");
        }

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

        // Allocate space for new line of code.
        size_t new_size = full_command_size + chars_read + 2;
        char *temp = realloc(full_command, new_size);
        if (!temp)
        {
            perror("realloc failure");
            free(line);
            free(full_command);
            return NULL;
        }
        full_command = temp;

        // Copy the new command.
        memcpy(full_command + full_command_size, line, chars_read);
        full_command_size += chars_read;

        // Separate new command with space char.
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