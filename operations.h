#include "tabla_simbolos.h"

float traverse_factor(struct node *root);
float traverse_term(struct node *root);
float traverse_expr(struct node *root);

struct DataItem **hashtable_global;
struct function **function_table;

void set_declarations(struct DataItem **hashtable, struct node *node)
{
    while (node != NULL)
    {
        if (node->nodeType != DECL)
        { //If not a decl
            yyerror("Error, no se encontro una declaracion");
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

float solve_sum(struct node *root)
{
    return traverse_expr(root->left) + traverse_term(root->right);
}

float solve_subs(struct node *root)
{
    return traverse_expr(root->left) - traverse_term(root->right);
}

float solve_mult(struct node *root)
{
    return traverse_term(root->left) * traverse_factor(root->right);
}

float solve_div(struct node *root)
{
    return traverse_term(root->left) / traverse_factor(root->right);
}

int solve_expression(struct node *root)
{
    if (root->nodeType == LESS_TH)
        return traverse_expr(root->left) < traverse_expr(root->right);
    if (root->nodeType == MORE_TH)
        return traverse_expr(root->left) > traverse_expr(root->right);
    if (root->nodeType == EQUAL_L)
        return traverse_expr(root->left) == traverse_expr(root->right);
    if (root->nodeType == LESS_EQ)
        return traverse_expr(root->left) <= traverse_expr(root->right);
    if (root->nodeType == MORE_EQ)
        return traverse_expr(root->left) >= traverse_expr(root->right);
    yyerror("Invalid expression");
    exit(1);
}

float traverse_factor(struct node *root)
{
    //(expr)
    if (root->left && root->left->nodeType == EXPR)
    {
        return traverse_expr(root->left);
    }
    //id
    if (root->left && root->left->nodeType == ID)
    {
        struct DataItem *data = getIdentValue(hashtable_global, root->left->id);
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
}

float traverse_term(struct node *root)
{
    if (root->left == NULL && root->right == NULL && root->other != NULL && root->other->nodeType == FACTOR)
    { //caso factor solo
        return traverse_factor(root->other);
    }
    if (root->left != NULL && root->left->nodeType == MULT_L)
    { //caso mult
        return solve_mult(root->left);
    }
    if (root->left != NULL && root->left->nodeType == DIV_L)
    { //caso div
        return solve_div(root->left);
    }
}

float traverse_expr(struct node *root)
{
    if (root->right == NULL && root->other == NULL && root->left != NULL && root->left->nodeType == TERM)
    { //caso term solo
        return traverse_term(root->left);
    }
    if (root->left != NULL && root->left->nodeType == SUM_L)
    { //caso sum
        return solve_sum(root->left);
    }
    if (root->left != NULL && root->left->nodeType == SUB_L)
    { //caso subs
        return solve_subs(root->left);
    }
    return -10000;
}

void check_types(int working_int, float tmp)
{
    if (working_int && floorf(tmp) != tmp)
    {
        yyerror("Invalid data types");
        exit(1);
    }
}

float traverse_opt_stmts(struct node *root)
{
    while (root != NULL)
    {
        struct node *stmnt = root->left;
        struct DataItem *data = NULL;
        float tmp = 0;
        switch (stmnt->nodeType)
        {
            case DECL:;
                data = getIdentValue(hashtable_global, stmnt->id);
                tmp = traverse_expr(stmnt->left);
                check_types(strcmp(data->dType, "int")==0, tmp);
                assignValue(hashtable_global, stmnt->id, data->dType, (int)tmp, tmp);
                break;
            case IF_FI:;
                if (solve_expression(stmnt->left))
                    traverse_opt_stmts(stmnt->right);
                break;
            case IF_ELSE:;
                if (solve_expression(stmnt->left))
                    traverse_opt_stmts(stmnt->right);
                else
                    traverse_opt_stmts(stmnt->other);
                break;
            case WHILE_L:;
                while (solve_expression(stmnt->left))
                    traverse_opt_stmts(stmnt->right);
                break;
            case FOR_L:;
                struct node *decl = stmnt->left;
                struct node *to_step = stmnt->right;

                data = getIdentValue(hashtable_global, decl->id);
                int working_int = strcmp(data->dType, "int") == 0 ? 1 : 0;
                tmp = traverse_expr(decl->left);
                check_types(working_int, tmp);
                assignValue(hashtable_global, decl->id, data->dType, (int)tmp, tmp);
                float to_expr = traverse_expr(to_step->left);
                float step_expr = traverse_expr(to_step->right);

                if (working_int && (floorf(to_expr) != to_expr || floorf(step_expr) != step_expr))
                {
                    yyerror("Invalid data types in for loop");
                    exit(1);
                }
                while ((working_int ? data->data : data->fdata) != to_expr)
                {
                    traverse_opt_stmts(stmnt->other);
                    if (working_int)
                        data->data += (int)step_expr;
                    else
                        data->fdata += step_expr;
                }
                break;
            case READ_L:;
                tmp = 0;
                data = getIdentValue(hashtable_global, stmnt->id);
                printf("Reading value of %s\n", stmnt->id);
                scanf("%f",&tmp);
                check_types(strcmp(data->dType, "int")==0, tmp);
                assignValue(hashtable_global, stmnt->id, data->dType, (int)tmp, tmp);
                break;
            case PRINT_L:
                tmp = traverse_expr(stmnt->left);
                if(floorf(tmp) == tmp)
                    printf("%d\n", (int)tmp);
                else
                    printf("%f\n", tmp);
                break;
            case RTRN:;
                return traverse_expr(root->left->left);
                break;
        }
        root = root->right; //Next stmnt
    }
    return FLT_MAX; //Void
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
    float result = traverse_opt_stmts(root->other);
    if(result!=(float)FLT_MAX)
        printf("Return = %f\n", result);
}