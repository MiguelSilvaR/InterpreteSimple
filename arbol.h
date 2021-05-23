#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

//Estructura del arbol
struct node
{
	char * id;
	int nodeType;
	int val_int;
	float val_float;
	struct node *left;
	struct node *right;
	struct node *other;
	/*
		0	id
		1   int
		2	float
		3 	expr
		4 	term
		5 	factor
		6	sum
		7	subst
		9 	stmnt
		10	decl
		11	asign
		12 	mult
		13 	div
	*/
};

struct node *create_node(char* id, int type, int val, float f, struct node *l, struct node *r, struct node *o)
{
	struct node *temp = (struct node *)malloc(sizeof(struct node));
	temp->id = id;
	temp->nodeType = type;
	temp->val_int = val;
	temp->val_float = f;
	temp->left = l;
	temp->right = r;
	temp->other = o;
	return temp;
}

void print_node(struct node *node){
	printf("Node[id=%s type=%d int=%d float=%f left=%p right=%p other=%p]\n",node->id,node->nodeType,node->val_int,node->val_float,node->left,node->right, node->other);
}

void print_node_value(struct node *node){
	if(node->nodeType==0)
		printf("%s\n",node->id);
	if(node->nodeType==1)
		printf("%f\n",node->val_float);
	if(node->nodeType==2)
		printf("%d\n",node->val_int);
}

void print_preorder(struct node *node){
	if(node!=NULL){
		print_node_value(node);
		print_preorder(node->left);
		print_preorder(node->right);
		print_preorder(node->other);
	}
}

void print_inorder(struct node *node){
	if(node!=NULL){
		print_inorder(node->left);
		print_node_value(node);
		print_inorder(node->right);
		print_inorder(node->other);
	}
}

void print_postorder(struct node *node){
	if(node!=NULL){
		print_postorder(node->left);
		print_postorder(node->right);
		print_postorder(node->other);
		print_node_value(node);
	}
}

void print_preorder_complete(struct node *node){
	if(node!=NULL){
		printf("%p ", node);
		print_node(node);
		print_preorder_complete(node->left);
		print_preorder_complete(node->right);
		print_preorder_complete(node->other);
	}
}