#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "tokenizer.h"

int main()
{
    do 
    {
        char *line_read = read_line();
        // Create dedicated check function for this.
        if (line_read == NULL)
        {
            printf("\n");
            free(line_read);
            exit(EXIT_FAILURE);
        }

        if (!strcmp(line_read, "exit"))
        {
            free(line_read);
            break;
        }

        printf("%s\n", line_read);
        
        // Free all allocated space.
        free(line_read);
    } while (true);


    return EXIT_SUCCESS;
}