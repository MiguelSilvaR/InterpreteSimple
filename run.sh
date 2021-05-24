#!/bin/bash
if [ -n "$1" ]; then
    file=$1
    if [ -n "$2" ]; then
        tc=$2
        cmd="flex tarea4.l && bison -d tarea4.y && gcc lex.yy.c tarea4.tab.c -lfl -lm && ./a.out $file < $tc && rm a.out"
        eval $cmd
    else
        cmd="flex tarea4.l && bison -d tarea4.y && gcc lex.yy.c tarea4.tab.c -lfl -lm && ./a.out $file && rm a.out"
        eval $cmd
    fi
else
    echo "Needs program to compile"
fi