cmd="flex tarea4.l && bison -d tarea4.y && gcc lex.yy.c tarea4.tab.c -lfl -lm && ./a.out in && rm a.out"

eval $cmd