#include "hashtable.h"
#include "arbol.h"

struct function{
    char* id;
    int return_type; //0 void, 1 int, 2 float
    struct node* root; //Opt_stmnts
    struct node* decl; //decl
    struct DataItem **hashtable_local;
};

struct table{
    struct function** function;
};

struct table* create_table(int size){
    return (struct table*)malloc(sizeof(struct table)*size);
}

void insert_function(struct node* root){
    struct function* f = (struct function*)malloc(sizeof(struct function));
    f->id = strdup(root->id);
    f->return_type = root->val_int;
    f->root = root->left;
    f->decl = root->right;
    f->hashtable_local = create_hashtable(depth_decl(f->decl));
}