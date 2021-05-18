/* Conjunto de funcionaes para el manejo de arboles sintacticos */

#include <stdio.h>
#include <stdlib.h>

struct treeNode
{
    int type;
    int value;
    struct treeNode *left;
    struct treeNode *right;
};


struct treeNode * createNode(int type, int num, struct treeNode *left, struct treeNode *right)
{
    struct treeNode *node = (struct treeNode *)malloc(sizeof(struct treeNode));
    return node;
}
