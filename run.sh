cmd="flex tarea4.l && bison -d tarea4.y && gcc lex.yy.c tarea4.tab.c -lfl"

eval $cmd