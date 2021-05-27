#include "hashtable.h"
#include "arbol.h"

struct function{
    char* id;
    int return_type; //0 void, 1 int, 2 float
    char** ids;
    struct node* params; //parameters
    struct node* stmnts; //Opt_stmnts
    struct node* decl; //decl
    struct DataItem **hashtable_local;
    struct DataItem **hashtable_original;
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

void set_variables(struct DataItem** hashtable, char** ids, struct node* node){
    int i = 0;
    while(node){
        ids[i++] = node->id;
        if (node->left->nodeType == INTT)
            declareVariable(hashtable, node->id, "int");
        if (node->left->nodeType == FLOATT)
            declareVariable(hashtable, node->id, "float");
        node = node->other;
    }
}


void insert_function_decl(struct function** table, struct node* root){
    int hashIndex = hashCode(root->id);
    if(search_f(table,root->id)!=NULL){
        printf("Double declaration of function\n");
        exit(1);
    }
    struct function* f = (struct function*)malloc(sizeof(struct function));
    f->id = strdup(root->id);
    f->return_type = root->val_int;
    f->ids = (char**)malloc(sizeof(char)*depth_other(f->params));
    f->params = root->left;
    f->hashtable_local = create_hashtable();
    set_variables(f->hashtable_local, f->ids, root->left);
    f->hashtable_original = copy_table(f->hashtable_local);
    insert_function(table, hashIndex, f);
}

void insert_function_def(struct function** table, struct node* root){
    struct function* f = search_f(table, root->id);
    int hashIndex = hashCode(root->id);
    if(f==NULL){
        struct function* f = (struct function*)malloc(sizeof(struct function));
        f->id = strdup(root->id);
        f->return_type = root->val_int;
        f->ids = (char**)malloc(sizeof(char)*depth_other(f->params));
        f->params = root->left;
        f->decl = root->right->left;
        f->stmnts = root->right->right;
        f->hashtable_local = create_hashtable();
        set_variables(f->hashtable_local, f->ids, root->left);
        f->hashtable_original = copy_table(f->hashtable_local);
        insert_function(table, hashIndex, f);
    }
    else{
        f->decl = root->right->left;
        f->stmnts = root->right->right;
    }
}

void print_table(struct function** f){
    int i = 0;
    int j = 0;
    for (i = 0; i < SIZE; i++)
    {
        if (f[i] != NULL){
            printf("%d - (%s, %s, params=[", i, f[i]->id, f[i]->return_type == INTT ? "int":"float");
            j=0;
            while(f[i]->ids[j+1])
                printf("%s,",f[i]->ids[j++]);
            printf("%s",f[i]->ids[j++]);
            printf("])\n");
        }
        else
            printf("%d - ..\n", i);
    }
    printf("\n");
}