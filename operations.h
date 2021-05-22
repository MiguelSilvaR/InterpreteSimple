#include "arbol.h"
#include "tabla_simbolos.h"

void set_declarations(struct node* node){
    while(node!=NULL){
        if(node->nodeType!=10){//If not a decl
            yyerror("Error, no se encontro una declaracion");
            exit(1);
        }
        if(node->left->nodeType==1){
            declareVariable(node->left->id, "int");
        }
        if(node->left->nodeType==2){
            declareVariable(node->left->id, "float");
        }
        node = node->right;
    }
}

float traverse_expression_float(struct node* root){
    return 123;
}

int traverse_expression_int(struct node* root){
    return 123;
}

void traverse_opt_stmts(struct node* root){
    while(root!=NULL){
        switch(root->nodeType){
            case 11:;
                struct DataItem *data = getIdentValue(root->id);
                print_node(root);
                if(strcmp(data->dType,"float")){
                    float tmp = traverse_expression_float(root);
                    assignValue(root->id, data->dType, 0, tmp);
                }
                else if(strcmp(data->dType,"int")){
                    int tmp = traverse_expression_int(root);
                    assignValue(root->id, data->dType, 0, tmp);
                }
                break;
        }
        root = root -> right;
    }
}

void traverse_tree(struct node* root){
    set_declarations(root->left);
    traverse_opt_stmts(root->right);
}