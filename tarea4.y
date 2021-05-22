/* Este archivo contiene un reconocedor de expresiones aritm�ticas junto
   con algunas reglas sem�nticas que calculan los valores de las
   expresiones que se reconocen. Las expresiones son muy sencillas y
   consisten �nicamente de sumas, restas, multiplicaciones y divisiones de
   n�meros enteros.

   Para compilar y ejecutar:
   flex calculadora.l
   bison calculadora.y
   gcc lex.yy.c calculadora.tab.c -lfl -lm
   ./a.out

    O de manera simplificada: 
    flex calculadora.l && bison -d calculadora.y && gcc lex.yy.c calculadora.tab.c -lfl -lm
    ./a.out

   Autor: Miguel Silva Ramos y Oscar Barbosa Aquino */

%{
#include "operations.h"

extern int yylex();
extern int yylineno;
extern FILE *yyin;
extern char * yytext;
int yyerror(char const * s);
struct node* root;

%}

/* Los elementos terminales de la gram�tica. La declaraci�n de abajo se
   convierte en definici�n de constantes en el archivo calculadora.tab.h
   que hay que incluir en el archivo de flex. */

%token BEGIN_STMT END_STMT IF FI ELSE WHILE 
        FOR TO STEP DO READ PRINT SEMICOLON ASSIGN_TYPE ASSIGN_VALUE 
        OPEN_PAREN CLOSE_PAREN SUM SUBST MULT DIV LESSTH MORETH EQUAL 
        LESSEQTH MOREEQTH 
%start prog

%union {
  struct node * node;
  char * tipo;
  int intVal;
  float floatVal;
}

%token <floatVal> FLOTANTE
%token <intVal> ENT 
%token <tipo> INT_DEF FLOAT_DEF IDENT

%type <tipo> tipo
%type <node> decl decl_lst opt_decls prog opt_stmts stmt stmt_lst expr term factor

%%


prog : opt_decls BEGIN_STMT opt_stmts END_STMT 
     {
          root = create_node("root", 0, 0, 0, $1, $3, NULL);;
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
               var = create_node($1,1,0,0,NULL,NULL,NULL);
          else if(strcmp($3, "float")==0)
               var = create_node($1,2,0,0,NULL,NULL,NULL);
          $$ = create_node(id,10,0,0,var,NULL,NULL);
     }
;

tipo : INT_DEF                                                   { $$ = "int"; }
     | FLOAT_DEF                                                 { $$ = "float"; }
;

opt_stmts : stmt_lst
          | %empty {$$ = NULL;}
;

stmt_lst : stmt SEMICOLON stmt_lst {}
         | stmt
;

stmt : IDENT ASSIGN_VALUE expr                                   
     { 
          $$ = create_node($1, 11, 0, 0, $3, NULL, NULL);
     }
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt FI
     {
          $$ = NULL;
     }
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt ELSE stmt
     {
          $$ = NULL;
     }
     | WHILE OPEN_PAREN expresion CLOSE_PAREN stmt
     {
          $$ = NULL;
     }
     | FOR IDENT ASSIGN_VALUE expr TO expr STEP expr DO stmt
     {
          $$ = NULL;
     }
     | READ IDENT
     {
          $$ = NULL;
     }
     | PRINT expr
     {
          $$ = NULL;
     }
     | BEGIN_STMT opt_stmts END_STMT
     {
          $$ = NULL;
     }
;



expr : expr SUM term                    
     | expr SUBST term                  
     | term
;

term : term MULT factor                 
     | term DIV factor                  
     | factor                           
;

factor : OPEN_PAREN expr CLOSE_PAREN 
          {
               $$ = NULL;
          }
       | IDENT                          
          {
               $$ = create_node($1, 0, 0, 0 , NULL,NULL,NULL);
          }
       | ENT                            
          {
               $$ = create_node("ent", 1, $1, 0 , NULL,NULL,NULL);
          }
       | FLOTANTE                       
          {
               $$ = create_node("flt_as", 2, 0, $1 , NULL,NULL,NULL);
          }
;

expresion : expr LESSTH expr
          | expr MORETH expr
          | expr EQUAL expr
          | expr LESSEQTH expr
          | expr MOREEQTH expr
;


%%

int yyerror(char const * s) {
     printf("Error, %s: '%s' in line %d\n", s, yytext, yylineno);
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
     printTable();
}

