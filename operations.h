#include "tabla_simbolos.h"

float traverse_factor(struct DataItem **hashtable, struct node *root);
float traverse_term(struct DataItem **hashtable, struct node *root);
float traverse_expr(struct DataItem **hashtable, struct node *root);
float solve_function(struct DataItem** hashtable, struct node *root);

struct DataItem **hashtable_global;
struct function **function_table;

void set_declarations(struct DataItem **hashtable, struct node *node)
{
    while (node != NULL)
    {
        if (node->nodeType != DECL)
        { //If not a decl
            printf("Error, no se encontro una declaracion\n");
            exit(1);
        }
        if (node->left->nodeType == INTT)
        {
            declareVariable(hashtable, node->left->id, "int");
        }
        if (node->left->nodeType == FLOATT)
        {
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
        struct DataItem *data = getIdentValue(hashtable, root->left->id);
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

float traverse_opt_stmts(struct DataItem **hashtable, struct node *root)
{
    while (root != NULL)
    {
        struct node *stmnt = root->left;
        struct DataItem *data = NULL;
        float tmp = 0;
        switch (stmnt->nodeType)
        {
            case DECL:;
                data = search(hashtable, stmnt->id);
                if(data!=NULL)
                    data->def++;
                data = getIdentValue(hashtable, stmnt->id);
                tmp = traverse_expr(hashtable, stmnt->left);
                check_types(strcmp(data->dType, "int")==0, tmp);
                assignValue(hashtable, stmnt->id, data->dType, (int)tmp, tmp);
                break;
            case IF_FI:;
                if (solve_expression(hashtable, stmnt->left))
                    traverse_opt_stmts(hashtable, stmnt->right);
                break;
            case IF_ELSE:;
                if (solve_expression(hashtable, stmnt->left))
                    traverse_opt_stmts(hashtable, stmnt->right);
                else
                    traverse_opt_stmts(hashtable, stmnt->other);
                break;
            case WHILE_L:;
                while (solve_expression(hashtable, stmnt->left))
                    traverse_opt_stmts(hashtable, stmnt->right);
                break;
            case FOR_L:;
                struct node *decl = stmnt->left;
                struct node *to_step = stmnt->right;

                data = getIdentValue(hashtable, decl->id);
                int working_int = strcmp(data->dType, "int") == 0 ? 1 : 0;
                tmp = traverse_expr(hashtable, decl->left);
                check_types(working_int, tmp);
                assignValue(hashtable, decl->id, data->dType, (int)tmp, tmp);
                float to_expr = traverse_expr(hashtable, to_step->left);
                float step_expr = traverse_expr(hashtable, to_step->right);

                if (working_int && (floorf(to_expr) != to_expr || floorf(step_expr) != step_expr))
                {
                    printf("Invalid data types in for loop\n");
                    exit(1);
                }
                while ((working_int ? data->data : data->fdata) != to_expr)
                {
                    traverse_opt_stmts(hashtable, stmnt->other);
                    if (working_int)
                        data->data += (int)step_expr;
                    else
                        data->fdata += step_expr;
                }
                break;
            case READ_L:;
                tmp = 0;
                data = search(hashtable, stmnt->id);
                if(data!=NULL)
                    data->def++;
                data = getIdentValue(hashtable, stmnt->id);
                printf("Reading value of %s\n", stmnt->id);
                scanf("%f",&tmp);
                check_types(strcmp(data->dType, "int")==0, tmp);
                assignValue(hashtable, stmnt->id, data->dType, (int)tmp, tmp);
                break;
            case PRINT_L:
                tmp = traverse_expr(hashtable, stmnt->left);
                if(floorf(tmp) == tmp)
                    printf("%d\n", (int)tmp);
                else
                    printf("%f\n", tmp);
                break;
            case RTRN:;
                return traverse_expr(hashtable, root->left->left);
                break;
        }
        root = root->right; //Next stmnt
    }
    return FLT_MAX; //Void
}

void set_parameters(struct DataItem** hashtable, struct function* function, struct node* expr){
    if(depth_right(expr)!=depth_other(function->params))
        printf("Error en el numero de parametros\n");
    for (size_t i = 0; i <= depth_right(expr); i++)
    {
        float val = traverse_expr(hashtable, expr);
        struct DataItem* data = getIdentValue(function->hashtable_local, function->ids[i]);
        check_types(strcmp(data->dType, "int")==0, val);
        assignValue(function->hashtable_local, data->key, data->dType, (int)val, val);
        expr = expr->right;
    }
}

float solve_function(struct DataItem** hashtable, struct node* root){
    struct function* function = search_f(function_table, root->id);
    set_parameters(hashtable, function, root->left);
    set_declarations(function->hashtable_local, function->decl);
    float result = traverse_opt_stmts(function->hashtable_local, function->stmnts);
    check_types(strcmp(function->return_type == 0 ? "int":"float", "int")==0, result);
    function->hashtable_local = copy_table(function->hashtable_original);
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
    set_declarations(hashtable_global, root->left);
    set_functions(root->right);
    float result = traverse_opt_stmts(hashtable_global, root->other);
    if(result!=(float)FLT_MAX)
        printf("Return = %f\n", result);
}