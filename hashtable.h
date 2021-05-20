#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 50

struct DataItem
{
    char * key;
    char * dType;
    int data;
    float fdata;
};

struct DataItem *hashArray[SIZE];
struct DataItem *dummyItem;
struct DataItem *item;

int hashCode(char *key)
{
    unsigned long hash = 5381;
    int c;
    while (c = *key++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % SIZE;
}

struct DataItem *search(char *key)
{
    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty
    while (hashArray[hashIndex] != NULL)
    {

        if (strcmp(hashArray[hashIndex]->key, key) == 0)
            return hashArray[hashIndex];

        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= SIZE;
    }

    return NULL;
}

void insert(char *key, int data, float fdata, char * dType)
{

    struct DataItem *item = (struct DataItem *)malloc(sizeof(struct DataItem));
    if (strcmp(dType, "float") == 0)
        item->fdata = fdata;
    else
        item->data = data;
    
    strcpy(item->key, key);
    strcpy(item->dType, dType);

    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty or deleted cell
    while (hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != NULL)
    {
        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= SIZE;
    }

    hashArray[hashIndex] = item;
}

struct DataItem *removeItem(struct DataItem *item)
{
    char *key;
    strcpy(key, item->key);

    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty
    while (hashArray[hashIndex] != NULL)
    {

        if (hashArray[hashIndex]->key == key)
        {
            struct DataItem *temp = hashArray[hashIndex];

            //assign a dummy item at deleted position
            hashArray[hashIndex] = dummyItem;
            return temp;
        }

        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= SIZE;
    }

    return NULL;
}

void display()
{
    int i = 0;

    for (i = 0; i < SIZE; i++)
    {

        if (hashArray[i] != NULL)
            printf(" (%s,%d)", hashArray[i]->key, hashArray[i]->data);
        else
            printf(" ~~ ");
    }

    printf("\n");
}