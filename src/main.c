#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

int main()
{
    do 
    {
        char *line_read = read_line();

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
        
        free(line_read);
    } while (true);


    return EXIT_SUCCESS;
}