#include "hashtable.h"
#include "arbol.h"

struct function{
    char* id;
    int return_type; //0 void, 1 int, 2 float
    struct node* params; //parameters
    struct node* stmnts; //Opt_stmnts
    struct node* decl; //decl
    struct DataItem **hashtable_local;
};

struct function* search_f(struct function** hashArray, char* id){
    int hashIndex = hashCode(id);
    while (hashArray[hashIndex] != NULL)
    {
        if (strcmp(hashArray[hashIndex]->id, id) == 0)
            return hashArray[hashIndex];
        ++hashIndex;
        hashIndex %= SIZE;
    }
    return NULL;
}

struct function** create_table_functions(){
    return (struct function**)malloc(sizeof(struct function)*SIZE);
}

void insert_function(struct function** table, int hashIndex, struct function* f){
    while (table[hashIndex] != NULL && table[hashIndex]->id != NULL)
    {
        ++hashIndex;
        hashIndex %= SIZE;
    }
    table[hashIndex] = f;
}

void insert_function_decl(struct function** table, struct node* root){
    int hashIndex = hashCode(root->id);
    if(search_f(table,root->id)!=NULL){
        printf("Double declaration of function %s", root->id);
        exit(1);
    }
    struct function* f = (struct function*)malloc(sizeof(struct function));
    f->id = strdup(root->id);
    f->return_type = root->val_int;
    f->params = root->left;
    f->hashtable_local = create_hashtable();
    insert_function(table, hashIndex, f);
}

void insert_function_def(struct function** table, struct node* root){
    struct function* f = search_f(table, root->id);
    int hashIndex = hashCode(root->id);
    if(f==NULL){
        struct function* f = (struct function*)malloc(sizeof(struct function));
        f->id = strdup(root->id);
        f->return_type = root->val_int;
        f->params = root->left;
        f->decl = root->right->left;
        f->stmnts = root->right->right;
        f->hashtable_local = create_hashtable();
        insert_function(table, hashIndex, f);
    }
    else{
        f->decl = root->right->left;
        f->stmnts = root->right->right;
    }
}

void print_table(struct function** f){
    int i = 0;
    for (i = 0; i < SIZE; i++)
    {
        if (f[i] != NULL)
            printf("%d - (%s, %s, params=%d)\n", i, f[i]->id, f[i]->return_type == INTT ? "int":"float", depth_other(f[i]->params));
        else
            printf("%d - ..\n", i);
    }
    printf("\n");
}