%{

/* Librerias utilizadas */
#include<stdio.h>
#include<stdlib.h>

#include "tarea4.tab.h"

/* Definimos algunas constantes para identificar fichas */

/* Palabras reservadas */
#define BEGIN_STMT 1 
#define END_STMT 2
#define INT_DEF 3
#define FLOAT_DEF 4
#define IF 5
#define FI 6
#define ELSE 7
#define WHILE 8
#define FOR 9
#define TO 10
#define STEP 11
#define DO 12
#define READ 13
#define PRINT 14

/* Simbolos y operaciones */
#define SEMICOLON 15
#define ASSIGN_TYPE 16
#define ASSIGN_VALUE 17
#define OPEN_PAREN 18
#define CLOSE_PAREN 19
#define SUM 20
#define SUBST 21
#define MULT 22
#define DIV 23
#define LESSTH 24
#define MORETH 25
#define EQUAL 26
#define LESSEQTH 27
#define MOREEQTH 28

/* Identificadores, tipo entero, tipo flotante y final de archivo */
#define IDENT 29
#define ENT 30
#define FLOTANTE 31 
#define END_OF_FILE 32

/* Auxiliares para detectar identaciones y espacios vacios */
#define BLANK 33
#define NEXT_LINE 34
#define UNKNOWN 35

%}
/* Reglas auxiliares */
LETRA [A-Za-z]
DIGITO [0-9]
NEXT[\n]
BLANKSPACE([ ])+

%%

"begin"                                         {return BEGIN_STMT;}
"end"                                           {return END_STMT;}
"int"                                           {return INT_DEF;}
"float"                                         {return FLOAT_DEF;}
"if"                                            {return IF;}
"fi"                                            {return FI;}
"else"                                          {return ELSE;}
"while"                                         {return WHILE;}
"for"                                           {return FOR;}
"to"                                            {return TO;}
"step"                                          {return STEP;}
"do"                                            {return DO;}
"read"                                          {return READ;}
"print"                                         {return PRINT;}

";"                                             {return SEMICOLON;}
":"                                             {return ASSIGN_TYPE;}
"<-"                                            {return ASSIGN_VALUE;}
"("                                             {return OPEN_PAREN;}
")"                                             {return CLOSE_PAREN;}
"+"                                             {return SUM;}
"-"                                             {return SUBST;}
"*"                                             {return MULT;}
"/"                                             {return DIV;}
"<"                                             {return LESSTH;}
">"                                             {return MORETH;}
"="                                             {return EQUAL;}
"<="                                            {return LESSEQTH;}
">="                                            {return MOREEQTH;}

({LETRA}|"_"|"$")({LETRA}|{DIGITO}|"_"|"$")*    {return IDENT;}
([1-9]{DIGITO}*)|0                              {return ENT;}
(([1-9]{DIGITO}*)|0)"."{DIGITO}{DIGITO}*        {return FLOTANTE;}

{NEXT}                                          {return NEXT_LINE;}
{BLANKSPACE}|[\r]                               {return BLANK;}
.                                               {return UNKNOWN;}

<<EOF>>                                         {return END_OF_FILE;}

%%
