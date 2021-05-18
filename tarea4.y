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
int yyerror(char const * s);

%}

/* Los elementos terminales de la gram�tica. La declaraci�n de abajo se
   convierte en definici�n de constantes en el archivo calculadora.tab.h
   que hay que incluir en el archivo de flex. */

%token BEGIN_STMT END_STMT INT_DEF FLOAT_DEF IF FI ELSE WHILE 
        FOR TO STEP DO READ PRINT SEMICOLON ASSIGN_TYPE ASSIGN_VALUE 
        OPEN_PAREN CLOSE_PAREN SUM SUBST MULT DIV LESSTH MORETH EQUAL 
        LESSEQTH MOREEQTH IDENT END_OF_FILE
%start exp

%union {
  int intVal;
  struct arbolNode * node;
  float floatVal;
}

%token <intVal> ENT
%token <floatVal> FLOTANTE


%%

exp : prog END_OF_FILE {  }
;

prog : opt_decls BEGIN_STMT opt_stmts END_STMT {  }
;

opt_decls : decl_lst {  }
          |          {  }
;

decl_lst : decl SEMICOLON decl_lst {  }
         | decl                    {  }
;

decl : IDENT ASSIGN_TYPE tipo {  }
;

tipo : INT_DEF    {  }
     | FLOAT_DEF  {  }
;

stmt : IDENT ASSIGN_VALUE expr
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt FI
     | IF OPEN_PAREN expresion CLOSE_PAREN stmt ELSE stmt
     | WHILE OPEN_PAREN expresion CLOSE_PAREN stmt
     | FOR IDENT ASSIGN_VALUE expr TO expr STEP expr DO stmt
     | READ IDENT
     | PRINT expr
     | BEGIN_STMT opt_stmts END_STMT
;

opt_stmts : stmt_lst
          |
;

stmt_lst : stmt SEMICOLON stmt_lst 
         | stmt
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
  fprintf(stderr, "%s\n", s);
}

void main() {

  yyparse();
}

