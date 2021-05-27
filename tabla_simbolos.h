#include "table.h"

int yyerror(char const * s);

struct DataItem * getIdentValue(struct DataItem **hashArray, char * ident) {
    struct DataItem * tmp = search(hashArray, ident);
    if(tmp==NULL){
        printf("Error, variable no declarada %s\n", ident);
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
}

void printTable(struct DataItem **hashArray) {
    display(hashArray);
}