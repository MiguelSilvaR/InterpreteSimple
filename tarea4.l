%{

/* Librerias utilizadas */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "tarea4.tab.h"


%}
/* Reglas auxiliares */
LETRA [A-Za-z]
DIGITO [0-9]
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
"fun"                                           {return FUN;}
"return"                                        {return RETURN;}

";"                                             {return SEMICOLON;}
":"                                             {return ASSIGN_TYPE;}
","                                             {return COMA;}
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

({LETRA}|"_"|"$")({LETRA}|{DIGITO}|"_"|"$")*    {yylval.tipo = strdup(yytext);return IDENT;}
([1-9]{DIGITO}*)|0                              {yylval.intVal = atoi(yytext);return ENT;}
(([1-9]{DIGITO}*)|0)"."{DIGITO}{DIGITO}*        {yylval.floatVal = atof(yytext);return FLOTANTE;}

"\n"                                            {yylineno++;}
{BLANKSPACE}|[\r]                               {}
%%