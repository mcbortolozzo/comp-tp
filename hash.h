#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TABLE_SIZE 997

typedef struct hashNode
{
    struct hashNode* next;
    char* text;
} hash_node_t;

hash_node_t* HashTable[TABLE_SIZE];

void hashInit();
hash_node_t* hashInsert(char *text);
hash_node_t* hashFind(char *text);
void hashPrint();
