#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 50

struct DataItem
{
    char *key;
    char *dType;
    int data;
    float fdata;
};

struct DataItem *dummyItem;
struct DataItem *item;

struct DataItem** create_hashtable(){
    return (struct DataItem**)malloc(sizeof(struct DataItem)*SIZE);
}

int hashCode(char *key)
{
    unsigned long hash = 5381;
    int c;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c;
    return hash % SIZE;
}

struct DataItem *search(struct DataItem **hashArray, char *key)
{
    int hashIndex = hashCode(key);
    while (hashArray[hashIndex] != NULL)
    {
        if (strcmp(hashArray[hashIndex]->key, key) == 0)
            return hashArray[hashIndex];
        ++hashIndex;
        hashIndex %= SIZE;
    }
    return NULL;
}

void insert(struct DataItem **hashArray, char *key, int data, float fdata, char *dType)
{
    struct DataItem *item = (struct DataItem *)malloc(sizeof(struct DataItem));
    if (strcmp(dType, "float") == 0)
        item->fdata = fdata;
    else
        item->data = data;
    item->key = strdup(key);
    item->dType = strdup(dType);
    int hashIndex = hashCode(key);
    while (hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != NULL)
    {
        ++hashIndex;
        hashIndex %= SIZE;
    }
    hashArray[hashIndex] = item;
}

struct DataItem *removeItem(struct DataItem **hashArray, char * key)
{
    int hashIndex = hashCode(key);
    while (hashArray[hashIndex] != NULL)
    {
        if (strcmp(hashArray[hashIndex]->key, key) == 0)
        {
            struct DataItem *temp = hashArray[hashIndex];
            hashArray[hashIndex] = dummyItem;
            return temp;
        }
        ++hashIndex;
        hashIndex %= SIZE;
    }
    return NULL;
}

void display(struct DataItem **hashArray)
{
    int i = 0;
    for (i = 0; i < SIZE; i++)
    {
        if (hashArray[i] != NULL)
            printf("%d - (%s, %d, %2f, %s)\n", i, hashArray[i]->key, hashArray[i]->data, hashArray[i]->fdata, hashArray[i]->dType);
        else
            printf("%d - ..\n", i);
    }
    printf("\n");
}