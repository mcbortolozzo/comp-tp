#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _HASH_MAP
#define _HASH_MAP

#define TABLE_SIZE 997

#define SYMBOL_LIT_INT    1
#define SYMBOL_LIT_REAL   2
#define SYMBOL_LIT_CHAR   3
#define SYMBOL_LIT_STRING 4
#define SYMBOL_IDENTIFIER 5


typedef struct hashNode
{
    struct hashNode* next;
    int type;
    int nature;
    int dataType;
    char* text;
} hash_node_t;

typedef struct hashMap
{
    hash_node_t ** nodes;
    int size;
} hash_map_t;

hash_node_t* HashTable[TABLE_SIZE];

hash_map_t* newHashMap(int tableSize);
hash_node_t* hashInsert(hash_map_t *map, int type, char *text);
hash_node_t* hashFind(hash_map_t *map, char *text);
void hashPrint(hash_map_t* table);

#endif
