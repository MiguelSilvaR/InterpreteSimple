#include "hashtable.h"

int yyerror(char const * s);

void declareVariable(char * ident, char * type) {
    insert(ident,0,0.0,type);
}

struct DataItem * getIdentValue(char * ident) {
    struct DataItem * tmp = search(ident);
    return tmp;
}

void checkTypes(char * type1, char * type2) {
    if (strcmp(type1, type2) != 0){
        yyerror("Error en tipos");
        exit(1);
    }
}

void assignValue(char * ident, char * typeExpression, int data, float fdata) {
    struct DataItem * tmp = search(ident);
    checkTypes(typeExpression, tmp->dType);
    if (strcmp(tmp->dType, "int") == 0)
        tmp->data = data;
    else
        tmp->fdata = fdata;
}

void printTable() {
    display();
}