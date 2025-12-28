#include <stdio.h>
#include <string.h>

#define BOX_LENGTH 60
#define BOX_WIDTH 3
#define BOX_LINE "═"
#define BOX_VERTICAL_LINE "║" 
#define TOP_LEFT_CORNER "╔"
#define TOP_RIGHT_CORNER "╗"
#define BOTTOM_LEFT_CORNER "╚"
#define BOTTOM_RIGHT_CORNER "╝"

void print_box_border(const char *left_corner_type, const char *right_corner_type);

void print_welcome_message(const char *message)
{
    print_box_border(TOP_LEFT_CORNER, TOP_RIGHT_CORNER);

    int message_len = strlen(message);
    int box_len;
    if (BOX_LENGTH > message_len)
    {
        box_len = BOX_LENGTH;
    }
    else
    {
        box_len = message_len + 2;
    }

    int left_padding = (box_len - message_len) / 2;
    int right_padding = box_len - message_len - left_padding;
    int vertical_center = BOX_WIDTH / 2;

    for (int i = 0; i < BOX_WIDTH; i++)
    {
        printf(BOX_VERTICAL_LINE);
        if (i == vertical_center)
        {
            for (int j = 0; j < left_padding; j++)
            {
                printf(" ");
            }
            printf("%s", message);
            for (int j = 0; j < right_padding; j++)
            {
                printf(" ");
            }
        }
        else
        {
            for (int j = 0; j < box_len; j++)
            {
                printf(" ");
            }
        }

        printf(BOX_VERTICAL_LINE"\n");
    }

    print_box_border(BOTTOM_LEFT_CORNER, BOTTOM_RIGHT_CORNER);
}

void print_box_border(const char *left_corner_type, const char *right_corner_type)
{
    printf("%s", left_corner_type);
    for (int i = 0; i < BOX_LENGTH; i++)
    {
        printf(BOX_LINE);
    }
    printf("%s\n", right_corner_type);
}
