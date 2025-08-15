#include <stdbool.h>
#include <stdio.h>

#include "command_syntax_tree.h"

void print_tree(ASTNode *node, int dist, bool node_end);
void print_tree_design(int dist, bool node_end);

#define BRANCH_END "└── "
#define BRANCH_REG "├── "
#define SPACE      "|   "

void print_ast(ASTNode *ast)
{
    int dist = 0;
    bool node_end = NULL;
    print_tree(ast, dist, node_end);
}


void print_tree(ASTNode *node, int dist, bool node_end)
{
    if (!node)
    {
        printf("There is no tree to print");
        return;
    }

    print_tree_design(dist, node_end);

    switch (node->type)
    {
        case NODE_COMMAND:
            printf("COMMAND:");
            for (int i = 0; node->command.argv[i]; i++)
            {
                printf(" %s", node->command.argv[i]);
            }
            printf("\n");
            break;

        case NODE_REDIRECT:
            switch (node->redirect.redirect_type)
            {
                case REDIRECT_INPUT:
                    printf("REDIRECT INPUT\n");
                    break;
                case REDIRECT_OUTPUT:
                    printf("REDIRECT OUTPUT\n");
                    break;
                case REDIRECT_APPEND:
                    printf("REDIRECT APPEND\n");
                    break;
            }
            printf("%s", node->redirect.filename);
            print_tree(node->redirect.child, dist + 1, false);
            break;

        case NODE_PIPELINE:
            printf("PIPE\n");
            print_tree(node->binary.left, dist + 1, false);
            print_tree(node->binary.right, dist + 1, true);
            break;
        
        case NODE_AND:
            printf("AND\n");
            print_tree(node->binary.left, dist + 1, false);
            print_tree(node->binary.right, dist + 1, true);
            break;
        
        case NODE_OR:
            printf("OR\n");
            print_tree(node->binary.left, dist + 1, false);
            print_tree(node->binary.right, dist + 1, true);
            break;

        case NODE_SEQUENCE:
            printf("SEQUENCE\n");
            print_tree(node->binary.left, dist + 1, false);
            print_tree(node->binary.right, dist + 1, true);
            break;

        case NODE_SUBSHELL:
            printf("SUBSHELL\n");
            print_tree(node->binary.left, dist + 1, true);
            break;
    }
}


void print_tree_design(int dist, bool node_end)
{
    for (int i = 0; i < dist; i++)
    {
        printf(SPACE);
    }

    if (node_end)
    {
        printf(BRANCH_END);
    }
    else
    {
        printf(BRANCH_REG);
    }
}