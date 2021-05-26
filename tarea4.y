/* Este archivo contiene un reconocedor de expresiones aritm�ticas junto
   con algunas reglas sem�nticas que calculan los valores de las
   expresiones que se reconocen. Las expresiones son muy sencillas y
   consisten �nicamente de sumas, restas, multiplicaciones y divisiones de
   n�meros enteros.

   Autor: Miguel Silva Ramos, Oscar Barbosa Aquino, Fernando Rodríguez Robles*/

%{
#include "operations.h"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
extern char * yytext;
int yyerror(char const * s);
struct node* root;
int flag_function_expr = 0;

%}

/* Los elementos terminales de la gram�tica. La declaraci�n de abajo se
   convierte en definici�n de constantes en el archivo calculadora.tab.h
   que hay que incluir en el archivo de flex. */

%token BEGIN_STMT END_STMT IF FI ELSE WHILE 
        FOR TO STEP DO READ PRINT SEMICOLON ASSIGN_TYPE ASSIGN_VALUE 
        OPEN_PAREN CLOSE_PAREN SUM SUBST MULT DIV LESSTH MORETH EQUAL 
        LESSEQTH MOREEQTH FUN RETURN COMA
%start prog

%union {
  struct node * node;
  char * tipo;
  int intVal;
  float floatVal;
}

%token <floatVal> FLOTANTE
%token <intVal> ENT 
%token <tipo>  INT_DEF FLOAT_DEF IDENT

%type <tipo>   tipo
%type <node>   decl decl_lst opt_decls prog opt_stmts stmt stmt_lst expr term factor expresion
               opt_fun_decl fun_decls fun_decl oparams params param opt_exprs expr_lst
          

%%


prog : opt_decls opt_fun_decl BEGIN_STMT opt_stmts END_STMT 
     {
          root = create_node("root", 0, 0, 0, $1, $2, $4);
     }
;

opt_decls : decl_lst
     | %empty            {$$ = NULL;}
;

decl_lst : decl SEMICOLON decl_lst
     {
          $1->right = $3;
          $$ = $1;
     }
     | decl
;

decl : IDENT ASSIGN_TYPE tipo                                    
     {
          char* id = (char*)malloc(sizeof(char)*10);
          struct node *var = (struct node *)malloc(sizeof(struct node));
          strcpy(id, "decl_");
          strcat(id, $1);
          if(strcmp($3, "int")==0)
               var = create_node($1,INTT,0,0,NULL,NULL,NULL);
          else if(strcmp($3, "float")==0)
               var = create_node($1,FLOATT,0,0,NULL,NULL,NULL);
          $$ = create_node(id,DECL,0,0,var,NULL,NULL);
     }
;

tipo : INT_DEF                                                   { $$ = "int"; }
     | FLOAT_DEF                                                 { $$ = "float"; }
;

opt_fun_decl : fun_decls
     | %empty            {$$ = NULL;}

fun_decls : fun_decl fun_decls
     {
          $1->other = $2;
          $$ = $1;
     }
     | fun_decl

fun_decl : FUN IDENT OPEN_PAREN oparams CLOSE_PAREN ASSIGN_TYPE tipo SEMICOLON
     {
          if(strcmp($7, "int") == 0)
               $$ = create_node($2, FN_DECL, INTT, 0, $4, NULL, NULL);
          else
               $$ = create_node($2, FN_DECL, FLOATT, 0, $4, NULL, NULL);
     }
     | FUN IDENT OPEN_PAREN oparams CLOSE_PAREN ASSIGN_TYPE tipo opt_decls BEGIN_STMT opt_stmts END_STMT
     {
          struct node* temp = create_node("optdecls_optstmnts", 0, 0, 0, $8, $10, NULL);
          if(strcmp($7, "int") == 0)
               $$ = create_node($2, FN_DEF, INTT, 0, $4, temp, NULL);
          else
               $$ = create_node($2, FN_DEF, FLOATT, 0, $4, temp, NULL);
     }

oparams : params
     | %empty            {$$ = NULL;}

params : param COMA params
     {
          $1->other = $3;
          $$ = $1;
     }
     | param

param : decl

opt_stmts : stmt_lst
          | %empty {$$ = NULL;}
;

stmt_lst : stmt SEMICOLON stmt_lst 
     {
          $1->right = $3;
          $$ = $1;
     }
     | stmt
;

stmt : IDENT ASSIGN_VALUE expr                                   
     { 
          struct node* tmp = create_node($1, DECL, 0, 0, $3, NULL, NULL);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt FI
     {
          struct node* tmp = create_node("IF_EXPR_S_FI", IF_FI, 0, 0, $3, $5, NULL);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt ELSE stmt
     {
          struct node* tmp = create_node("IF_ELSE", IF_ELSE, 0, 0, $3, $5, $7);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
     | WHILE OPEN_PAREN expresion CLOSE_PAREN stmt
     {
          struct node* tmp = create_node("WHILE", WHILE_L, 0, 0, $3, $5, NULL);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
     | FOR IDENT ASSIGN_VALUE expr TO expr STEP expr DO stmt
     {
          struct node* decl = create_node($2, DECL, 0, 0, $4, NULL, NULL);
          struct node* to_step = create_node("to_step", TO_STEP, 0, 0, $6, $8, NULL);
          struct node* tmp = create_node("FOR", FOR_L, 0, 0, decl, to_step, $10);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
     | READ IDENT
     {
          struct node* tmp = create_node($2, READ_L, 0, 0, NULL, NULL, NULL);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
     | PRINT expr
     {
          struct node* tmp = create_node("print_expr", PRINT_L, 0, 0, $2, NULL, NULL);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
     | BEGIN_STMT opt_stmts END_STMT
     {
          $$ = $2;
     }
     | RETURN expr
     {
          struct node* tmp = create_node("return", RTRN, 0, 0, $2, NULL, NULL);
          $$ = create_node("stmnt", STMNT, 0, 0, tmp, NULL, NULL);
     }
;

expr : expr SUM term
     {
          struct node* sum = create_node("sum", SUM_L, 0, 0, $1, $3, NULL);
          $$ = create_node("expr", EXPR, 0, 0, sum, NULL, NULL);
     }
     | expr SUBST term
     {
          struct node* subst = create_node("subst", SUB_L, 0, 0, $1, $3, NULL);
          $$ = create_node("expr", EXPR, 0, 0, subst, NULL, NULL);
     }
     | term
     {
          $$ = create_node("expr", EXPR, 0, 0, $1, NULL, NULL);
     }
;

term : term MULT factor
     {
          struct node* mult = create_node("mult", MULT_L, 0, 0, $1, $3, NULL);
          $$ = create_node("term", TERM, 0, 0, mult, NULL, NULL);
     }
     | term DIV factor
     {
          struct node* div = create_node("div", DIV_L, 0, 0, $1, $3, NULL);
          $$ = create_node("term", TERM, 0, 0, div, NULL, NULL);
     }
     | factor
     {
          $$ = create_node("term", TERM, 0, 0, NULL, NULL, $1);
     }                           
;

factor : OPEN_PAREN expr CLOSE_PAREN 
          {
               $$ = create_node("factor", FACTOR, 0, 0 , $2, NULL ,NULL);; //Todo
          }
     | IDENT                          
          {
               struct node* temp = create_node($1, 0, 0, 0 , NULL,NULL,NULL);
               $$ = create_node("factor", FACTOR, 0, 0 , temp, NULL, NULL); //Regresar un identificador
          }
     | ENT                            
          {
               struct node* temp = create_node("int", INTT, $1, 0, NULL,NULL,NULL);
               $$ = create_node("factor", FACTOR, 0, 0 , temp, NULL, NULL);
          }
     | FLOTANTE                       
          {
               struct node* temp = create_node("float", FLOATT, 0, $1, NULL,NULL,NULL);
               $$ = create_node("factor", FACTOR, 0, 0 , temp, NULL, NULL);
          }
     | IDENT OPEN_PAREN opt_exprs CLOSE_PAREN
          {
               struct node* temp = create_node($1, FN_CALL, 0, 0, $3, NULL,NULL);
               $$ = create_node("factor", FACTOR, 0, 0 , temp, NULL, NULL);
          }
;

opt_exprs : expr_lst

expr_lst : expr COMA expr_lst
     {
          $1->right = $3;
          $$ = $1;
     }
     | expr

expresion : expr LESSTH expr
          {
               $$ = create_node("lessthan", LESS_TH, 0, 0, $1, $3, NULL);
          }
          | expr MORETH expr
          {
               $$ = create_node("moreth", MORE_TH, 0, 0, $1, $3, NULL);
          }
          | expr EQUAL expr
          {
               $$ = create_node("equal", EQUAL_L, 0, 0, $1, $3, NULL);
          }
          | expr LESSEQTH expr
          {
               $$ = create_node("less_equal", LESS_EQ, 0, 0, $1, $3, NULL);
          }
          | expr MOREEQTH expr
          {
               $$ = create_node("more_equal", MORE_EQ, 0, 0, $1, $3, NULL);
          }
;


%%

int yyerror(char const * s) {
     printf("Error, %s: '%s' in line %d\n", s, yytext, yylineno);
     exit(1);
}

void main(int argc, char **argv) {
     if (argc < 2) {
          printf ("Error, falta el nombre de un archivo\n");
          exit(1);
     }

     yyin = fopen(argv[1], "r");

     if (yyin == NULL) {
          printf("Error: el archivo no existe\n");
          exit(1);
     }
     yyparse();
     traverse_tree(root);
}

