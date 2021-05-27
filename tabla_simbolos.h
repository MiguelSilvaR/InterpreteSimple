#include "table.h"

int yyerror(char const * s);

struct DataItem * getIdentValue(struct DataItem **hashArray, char * ident) {
    struct DataItem * tmp = search(hashArray, ident);
    if(tmp==NULL || tmp->def == 0){
        printf("Error, variable no definida %s\n", ident);
        exit(1);
    }
    return tmp;
}

void checkTypes(char * type1, char * type2) {
    if (strcmp(type1, type2) != 0){
        yyerror("Error en tipos");
        exit(1);
    }
}

void assignValue(struct DataItem **hashArray, char * ident, char * typeExpression, int data, float fdata) {
    struct DataItem * tmp = search(hashArray, ident);
    checkTypes(typeExpression, tmp->dType);
    if (strcmp(tmp->dType, "int") == 0)
        tmp->data = (int)data;
    else
        tmp->fdata = fdata;
    tmp->def = 1;
}

void printTable(struct DataItem **hashArray) {
    display(hashArray);
}