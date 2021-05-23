#include "arbol.h"
#include "tabla_simbolos.h"

float traverse_factor(struct node* root);
float traverse_term(struct node* root);
float traverse_expression(struct node* root);

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

float solve_sum(struct node* root){
    return traverse_expression(root->left) + traverse_term(root->right);
}

float solve_subs(struct node* root){
    return traverse_expression(root->left) - traverse_term(root->right);
}

float traverse_factor(struct node* root){
    //(expr)
    if(root->right && root->right->nodeType == 3){
        return traverse_expression(root->right);
    }
    //id
    if(root->right && root->right->nodeType == 0){
        struct DataItem* data = getIdentValue(root->right->id);
        if (strcmp(data->dType, "float")==0)
            return data->fdata;
        else
            return data->data;
    }
    //INT
    if(root->right && root->right->nodeType == 1){
        return root->right->val_int;
    }
    //FLOAT
    if(root->right && root->right->nodeType == 2){
        return root->right->val_float;
    }
    yyerror("Error in traverse factor");
    exit(1);
}

float traverse_term(struct node* root){
    if(root->left==NULL && root->right==NULL && root->other!=NULL && root->other->nodeType == 5){//caso factor solo
        return traverse_factor(root->other);
    }
}

float traverse_expression(struct node* root){
    if(root->left==NULL && root->right==NULL && root->other!=NULL && root->other->nodeType == 4){//caso term solo
        return traverse_term(root->other);
    }
    if(root->left!=NULL && root->left->nodeType == 6){//caso sum
        return solve_sum(root->left);
    }
    if(root->left!=NULL && root->left->nodeType == 7){//caso subs
        return solve_subs(root->left);
    }
    return 123;
}

void traverse_opt_stmts(struct node* root){
    print_preorder_complete(root);
    while(root!=NULL){
        struct node* stmnt = root->left;
        switch(stmnt->nodeType){
            case 11:;
                struct DataItem *data = getIdentValue(stmnt->id);
                float tmp = traverse_expression(stmnt->left);
                if(strcmp(data->dType, "int")==0 && floorf(tmp) != tmp){
                    yyerror("Invalid data types");
                    exit(1);
                }
                assignValue(stmnt->id, data->dType, (int)tmp, tmp);
                break;
        }
        root = root->right;//Next stmnt
    }
}

void traverse_tree(struct node* root){
    set_declarations(root->left);
    traverse_opt_stmts(root->right);
}