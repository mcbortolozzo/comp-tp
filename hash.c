#include "hash.h"

int hashString(char *text);
void printRow(hash_node_t *start);


int hashString(char *text)
{
    int i, hash = 1;
    int textLength = strlen(text);
    for(i = 0; i < textLength; i++)
    {
        hash = text[i]*hash % TABLE_SIZE;
    }
    return hash - 1;
}

void hashInit()
{
    //TODO
}

hash_node_t* hashInsert(char *text)
{
    hash_node_t *node = hashFind(text);

    if(node != 0)
    {
        return node;
    }

    node = calloc(1, sizeof(hash_node_t));
    node->text = calloc(strlen(text) + 1, sizeof(char));
    strcpy(node->text, text);

    int hash = hashString(text);
    
    hash_node_t *curr = HashTable[hash];
    if(curr == NULL)
    {
        HashTable[hash] = node;
    } 
    else
    {
        hash_node_t* prev;
        while(curr != NULL)
        {
            prev = curr;
            curr = curr->next;
        }
        prev->next = node;        
    }

    return node;
}

hash_node_t *hashFind(char *text)
{
    int hash = hashString(text);
    
    hash_node_t *curr = HashTable[hash];
    while(curr != NULL)
    {
        if(strcmp(curr->text, text) == 0)
            return curr;
    }
    return 0;
}

void hashPrint()
{
    int i;
    for(i = 0; i < TABLE_SIZE; i++)
    {
        if(HashTable[i] != NULL)
        {
            printf("Row:%d\n", i);
            printRow(HashTable[i]);   
        }
    }         
}

void printRow(hash_node_t *start)
{
    hash_node_t *curr = start;
    while(curr != NULL)
    {
        printf("\t%s\n", curr->text);
        curr = curr->next;
    }
}

