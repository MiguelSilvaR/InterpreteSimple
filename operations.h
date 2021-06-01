#include "tabla_simbolos.h"

float traverse_factor(struct DataItem **hashtable, struct node *root);
float traverse_term(struct DataItem **hashtable, struct node *root);
float traverse_expr(struct DataItem **hashtable, struct node *root);
float solve_function(struct DataItem** hashtable, struct node *root);

struct DataItem **hashtable_global;
struct DataItem **hashtable_main;
struct function **function_table;

char* parse(char* str){
    int index = 1;
    while(str[index+1]!='\0'){
        if (str[index]=='\\' && str[index+1] == 'n')
        {
            printf("\n");
            index+=2;
        }        
        else{
            printf("%c",str[index++]);
        }
    }
}

void set_data(struct DataItem **hashtable, char* key, char* dType, int ival, float fval){
    struct DataItem* tmp = search(hashtable, key);
    if(tmp){
        assignValue(hashtable, key, tmp->dType, (int)ival, fval);
    }
    else{
        tmp = search(hashtable_global, key);
        if(!tmp){
            printf("set_data: Variable not found %s\n",key);
            exit(1);
        }
        assignValue(hashtable_global, key, tmp->dType, (int)ival, fval);
    }
}

struct DataItem *get_data(struct DataItem **hashtable, char* key){
    struct DataItem* tmp = search(hashtable, key);
    if(tmp){
        if(tmp->def>0)
            return tmp;
        else{
            printf("get_data: Variable not defined %s\n", key);
            exit(1);
        }
    }
    else{
        tmp = search(hashtable_global, key); //Buscar en global
        if(tmp==NULL)
            return NULL;
        if(tmp->def==0){
            printf("get_data: Variable not declared %s\n", key);
            exit(1);
        }
        return tmp;
    }
}

struct DataItem *get_data_nf(struct DataItem **hashtable, char* key){
    struct DataItem* tmp = search(hashtable, key);
    if(tmp){
        return tmp;
    }
    else{
        tmp = search(hashtable_global, key); //Buscar en global
        if(!tmp){
            printf("get_data: Variable not declared %s\n", key);
            exit(1);
        }
        return tmp;
    }
}

void def(struct DataItem **hashtable, char* key){
    struct DataItem* tmp = search(hashtable, key);
    if(tmp)
        tmp->def++;
    else{
        tmp = search(hashtable_global, key); //Buscar en global
        if(tmp)
            tmp->def++;
        else{
            printf("def: Variable not declared %s\n", key);
            exit(1);
        }
    }
}

void set_declarations(struct DataItem **hashtable, struct node *node)
{
    struct DataItem *data;
    while (node != NULL)
    {
        if (node->nodeType != DECL)
        { //If not a decl
            printf("Error, no se encontro una declaracion\n");
            exit(1);
        }
        if (node->left->nodeType == INTT){
            data = search(hashtable_global, node->left->id);
            if(data){
                printf("Variable already declared in global scope %s\n", node->left->id);
                exit(1);
            }
            declareVariable(hashtable, node->left->id, "int");
        }
        if (node->left->nodeType == FLOATT){
            data = search(hashtable_global, node->left->id);
            if(data){
                printf("Variabla already declared in global scope %s", node->left->id);
                exit(1);
            }
            declareVariable(hashtable, node->left->id, "float");
        }
        node = node->right;
    }
}

float solve_sum(struct DataItem **hashtable, struct node *root)
{
    return traverse_expr(hashtable, root->left) + traverse_term(hashtable, root->right);
}

float solve_subs(struct DataItem **hashtable, struct node *root)
{
    return traverse_expr(hashtable, root->left) - traverse_term(hashtable, root->right);
}

float solve_mult(struct DataItem **hashtable, struct node *root)
{
    return traverse_term(hashtable, root->left) * traverse_factor(hashtable, root->right);
}

float solve_div(struct DataItem **hashtable, struct node *root)
{
    return traverse_term(hashtable, root->left) / traverse_factor(hashtable, root->right);
}

int solve_expression(struct DataItem **hashtable, struct node *root)
{
    if (root->nodeType == LESS_TH)
        return traverse_expr(hashtable, root->left) < traverse_expr(hashtable, root->right);
    if (root->nodeType == MORE_TH)
        return traverse_expr(hashtable, root->left) > traverse_expr(hashtable, root->right);
    if (root->nodeType == EQUAL_L)
        return traverse_expr(hashtable, root->left) == traverse_expr(hashtable, root->right);
    if (root->nodeType == LESS_EQ)
        return traverse_expr(hashtable, root->left) <= traverse_expr(hashtable, root->right);
    if (root->nodeType == MORE_EQ)
        return traverse_expr(hashtable, root->left) >= traverse_expr(hashtable, root->right);
    printf("Invalid expression\n");
    exit(1);
}

float traverse_factor(struct DataItem **hashtable, struct node *root)
{
    //(expr)
    if (root->left && root->left->nodeType == EXPR)
    {
        return traverse_expr(hashtable, root->left);
    }
    //id
    if (root->left && root->left->nodeType == ID)
    {
        struct DataItem *data = get_data(hashtable, root->left->id);
        if (strcmp(data->dType, "float") == 0)
            return data->fdata;
        else
            return data->data;
    }
    //INT
    if (root->left && root->left->nodeType == INTT)
    {
        return root->left->val_int;
    }
    //FLOAT
    if (root->left && root->left->nodeType == FLOATT)
    {
        return root->left->val_float;
    }
    //Function
    if (root->left && root->left->nodeType == FN_CALL)
    {  
        return solve_function(hashtable, root->left);
    }
    
}

float traverse_term(struct DataItem **hashtable, struct node *root)
{
    if (root->left == NULL && root->right == NULL && root->other != NULL && root->other->nodeType == FACTOR)
    { //caso factor solo
        return traverse_factor(hashtable, root->other);
    }
    if (root->left != NULL && root->left->nodeType == MULT_L)
    { //caso mult
        return solve_mult(hashtable, root->left);
    }
    if (root->left != NULL && root->left->nodeType == DIV_L)
    { //caso div
        return solve_div(hashtable, root->left);
    }
}

float traverse_expr(struct DataItem **hashtable, struct node *root)
{
    if (root->other == NULL && root->left != NULL && root->left->nodeType == TERM)
    { //caso term solo
        return traverse_term(hashtable, root->left);
    }
    if (root->left != NULL && root->left->nodeType == SUM_L)
    { //caso sum
        return solve_sum(hashtable, root->left);
    }
    if (root->left != NULL && root->left->nodeType == SUB_L)
    { //caso subs
        return solve_subs(hashtable, root->left);
    }
    printf("Bad expresion, traverse\n");
    exit(1);
}

void check_types(int working_int, float tmp)
{
    if (working_int && floorf(tmp) != tmp)
    {
        printf("Invalid data types\n");
        exit(1);
    }
}

void check_if_def(struct DataItem* tmp, char* key){
    if(tmp==NULL || tmp->def == 0){
        printf("Error, variable not defined %s\n", key);
        exit(1);
    }
}

float traverse_opt_stmts(struct DataItem **hashtable, struct node *root)
{
    while (root != NULL)
    {
        struct node *stmnt = root->left;
        float tmp = 0;
        struct DataItem *data;
        switch (stmnt->nodeType)
        {
            case DECL:;
                tmp = traverse_expr(hashtable, stmnt->left);
                data = get_data_nf(hashtable, stmnt->id);
                if(data){
                    check_types(strcmp(data->dType, "int")==0, tmp);
                    set_data(hashtable, stmnt->id, data->dType, (int)tmp, tmp);
                    def(hashtable, stmnt->id);
                }
                else{
                    printf("No se encontro variable %s\n", stmnt->id);
                    exit(1);
                }
                break;
            case IF_FI:;
                if (solve_expression(hashtable, stmnt->left)){
                    tmp = traverse_opt_stmts(hashtable, stmnt->right);
                    if(tmp!=FLT_MAX) return tmp;
                }
                break;
            case IF_ELSE:;
                if (solve_expression(hashtable, stmnt->left)){
                    tmp = traverse_opt_stmts(hashtable, stmnt->right);
                    if(tmp!=FLT_MAX) return tmp;
                }
                else{
                    tmp = traverse_opt_stmts(hashtable, stmnt->other);
                    if(tmp!=FLT_MAX) return tmp;
                }
                break;
            case WHILE_L:;
                while (solve_expression(hashtable, stmnt->left)){
                    tmp = traverse_opt_stmts(hashtable, stmnt->right);
                    if(tmp!=FLT_MAX) return tmp;
                }
                break;
            case FOR_L:;
                struct node *decl = stmnt->left;
                struct node *to_step = stmnt->right;
                
                tmp = traverse_expr(hashtable, decl->left);
                data = get_data_nf(hashtable, decl->id);
                int working_int = strcmp(data->dType, "int") == 0 ? 1 : 0;                
                
                if(data){
                    check_types(working_int, tmp);
                    set_data(hashtable, decl->id, data->dType, (int)tmp, tmp);
                    def(hashtable, decl->id);
                }

                float to_expr = traverse_expr(hashtable, to_step->left);
                float step_expr = traverse_expr(hashtable, to_step->right);

                if (working_int && (floorf(to_expr) != to_expr || floorf(step_expr) != step_expr))
                {
                    printf("Invalid data types in for loop\n");
                    exit(1);
                }
                while ((working_int ? data->data : data->fdata) != to_expr)
                {
                    tmp = traverse_opt_stmts(hashtable, stmnt->other);
                    if(tmp!=FLT_MAX){
                        return tmp;
                    }
                    if (working_int)
                        data->data += (int)step_expr;
                    else
                        data->fdata += step_expr;
                }
                break;
            case READ_L:;
                char* str = (char *)malloc(sizeof(char)*15);
                scanf("%s",str);
                float number = atof(str);
                def(hashtable, stmnt->id);
                data = get_data(hashtable, stmnt->id);
                check_types(strcmp(data->dType, "int")==0, number);
                set_data(hashtable, stmnt->id, data->dType, (int)number, number);
                break;
            case PRINT_L:
                if (stmnt->val_int==2)
                    parse(stmnt->id);
                else{
                    tmp = traverse_expr(hashtable, stmnt->left);
                    if(floorf(tmp) == tmp)
                        if(stmnt->val_int)
                            printf("%d", (int)tmp);
                        else
                            printf("%d\n", (int)tmp);
                    else
                        if(stmnt->val_int)
                            printf("%f", tmp);
                        else
                            printf("%f\n", tmp);
                }
                break;
            case RTRN:;
                return traverse_expr(hashtable, root->left->left);
        }
        root = root->right; //Next stmnt
    }
    return FLT_MAX; //Void
}

void set_parameters(struct DataItem** hashtable, struct DataItem** local_table, struct function* function, struct node* expr){
    if(depth_right(expr)!=depth_other(function->params))
        printf("Error en el numero de parametros\n");
    for (size_t i = 0; i <= depth_right(expr); i++)
    {        
        float val = traverse_expr(hashtable, expr);
        struct DataItem* data = get_data_nf(local_table, function->ids[i]);
        if(data){
            check_types(strcmp(data->dType, "int")==0, val);
            set_data(local_table, data->key, data->dType, (int)val, val);
            def(local_table, function->ids[i]);
        }
        expr = expr->right;
    }
}

float solve_function(struct DataItem** hashtable, struct node* root){
    struct function* function = search_f(function_table, root->id);
    struct DataItem** local_table = copy_table(function->hashtable);
    if(function==NULL){
        printf("Funcion no declarada, %s\n", root->id);
        exit(1);
    }
    set_parameters(hashtable, local_table, function, root->left);
    set_declarations(local_table, function->decl);
    if(function->stmnts==NULL){
        printf("Function not defined %s\n", function->id);
        exit(1);
    }
    float result = traverse_opt_stmts(local_table, function->stmnts);
    check_types(strcmp(function->return_type == 0 ? "int":"float", "int")==0, result);
    return function->return_type == 0 ? (int)result:result;
}

void set_functions(struct node* root){
    if(root==NULL)
        return;
    function_table = create_table_functions();
    while(root){
        switch(root->nodeType){
            case FN_DECL:
                insert_function_decl(function_table, root);
                break;
            case FN_DEF:
                insert_function_def(function_table, root);
                break;
        }
        root = root->other;
    }
}

void traverse_tree(struct node *root)
{
    // print_preorder_complete(root);
    hashtable_global = create_hashtable();
    hashtable_main = create_hashtable();
    set_declarations(hashtable_global, root->left);
    set_functions(root->right);
    float result = traverse_opt_stmts(hashtable_main, root->other);
    if(result!=(float)FLT_MAX)
        printf("Return = %f\n", result);
}