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
#include<stdio.h>

#include "arbol.h"
#include "tabla_simbolos.h"

extern int yylex();
extern int yylineno;
extern char * yytext;
int yyerror(char const * s);

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
  struct treeNode * node;
  char * tipo;
  int intVal;
  float floatVal;
}

%token <floatVal> FLOTANTE
%token <intVal> ENT 
%token <tipo> INT_DEF FLOAT_DEF IDENT

%type <tipo> tipo

%%


prog : opt_decls BEGIN_STMT opt_stmts END_STMT                   { printTable(); }
;

opt_decls : decl_lst
          | %empty
;

decl_lst : decl SEMICOLON decl_lst 
         | decl                    
;

decl : IDENT ASSIGN_TYPE tipo                                    { declareVariable($1,$3); }
;

tipo : INT_DEF                                                   { $$ = "int"; }
     | FLOAT_DEF                                                 { $$ = "float"; }
;

opt_stmts : stmt_lst
          | %empty
;

stmt_lst : stmt SEMICOLON stmt_lst 
         | stmt
;

stmt : IDENT ASSIGN_VALUE expr                                   { assignValue($1, "float" ,(int)6.1, (float)6.1); }
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt FI
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt ELSE stmt
     | WHILE OPEN_PAREN expresion CLOSE_PAREN stmt
     | FOR IDENT ASSIGN_VALUE expr TO expr STEP expr DO stmt
     | READ IDENT
     | PRINT expr
     | BEGIN_STMT opt_stmts END_STMT
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
       | IDENT                          
       | ENT                            
       | FLOTANTE                       
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

void main() {

  yyparse();
}

