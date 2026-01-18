#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command_syntax_tree.h"

void print_tree(ASTNode *node, bool node_end, char *outline);
void print_tree_design(int dist, bool node_end);
char *create_new_outline(char *outline, bool node_end);

#define BRANCH_END "└── "
#define BRANCH_REG "├── "
#define SPACE "    "
#define SEPARATOR "|   "
#define INDENT_SIZE 5

void print_ast(ASTNode *ast)
{
    bool node_end = true;
    char *outline = "";
    print_tree(ast, node_end, outline);
}

void print_tree(ASTNode *node, bool node_end, char *outline)
{
    if (!node) {
        printf("There is no tree to print");
        return;
    }

    printf("%s", outline);
    if (node_end) {
        printf("%s", BRANCH_END);
    } else {
        printf("%s", BRANCH_REG);
    }

    char *new_outline = create_new_outline(outline, node_end);
    if (!new_outline) {
        fprintf(stderr, "print_tree: malloc failure");
        return;
    }

    if (node->type == NODE_COMMAND) {
        printf("COMMAND:");
        for (int i = 0; node->command.argv[i]; i++) {
            printf(" %s", node->command.argv[i]);
        }
        printf("\n");
    }

    else if (node->type == NODE_REDIRECT) {
        switch (node->redirect.redirect_type) {
        case REDIRECT_INPUT:
            printf("REDIRECT INPUT <");
            break;
        case REDIRECT_OUTPUT:
            printf("REDIRECT OUTPUT >");
            break;
        case REDIRECT_APPEND:
            printf("REDIRECT APPEND >>");
            break;
        }

        printf(" %s\n", node->redirect.filename);
        print_tree(node->redirect.child, true, new_outline);
    }

    else if (node->type == NODE_PIPELINE || node->type == NODE_AND || node->type == NODE_OR ||
             node->type == NODE_SEQUENCE) {
        switch (node->type) {
        case NODE_PIPELINE:
            printf("PIPE\n");
            break;
        case NODE_AND:
            printf("AND\n");
            break;
        case NODE_OR:
            printf("OR\n");
            break;
        case NODE_SEQUENCE:
            printf("SEQUENCE\n");
            break;
        default:
            return;
        }
        print_tree(node->binary.left, false, new_outline);
        print_tree(node->binary.right, true, new_outline);
    }

    else if (node->type == NODE_SUBSHELL) {
        printf("SUBSHELL\n");
        print_tree(node->binary.left, true, new_outline);
    }

    free(new_outline);
}

char *create_new_outline(char *outline, bool node_end)
{
    size_t new_outline_size = strlen(outline) + INDENT_SIZE;
    char *new_outline = malloc(new_outline_size);
    if (!new_outline) {
        return NULL;
    }

    strcpy(new_outline, outline);

    if (node_end) {
        strcat(new_outline, SPACE);
    } else {
        strcat(new_outline, SEPARATOR);
    }

    return new_outline;
}
