#!/bin/bash
N=`ls \tests | wc -l`
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
for (( c=1; c<=$N/3; c++ ))
do
    eval "flex teemo.l && bison -d teemo.y && gcc lex.yy.c teemo.tab.c -lfl -lm && ./a.out tests/prueba${c}.txt < tests/prueba${c}_in.txt > tests/out${c}.txt && rm a.out"
    file1="tests/prueba${c}_out.txt"
    file2="tests/out${c}.txt"
    if cmp -s "$file1" "$file2"; then
        printf "TEST${c}\t${GREEN}passed\n${NC}"
    else
        printf "TEST${c}\t${RED}failed\n${NC}"
    fi
    eval "rm tests/out${c}.txt"
done