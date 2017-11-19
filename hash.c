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

hash_map_t* newHashMap(int size)
{
	hash_node_t** table = calloc(size, sizeof(hash_node_t));
	hash_map_t* map = calloc(1, sizeof(hash_map_t));
	map->size = size;
	map->nodes = table;
	return map;
}

hash_node_t* hashInsert(hash_map_t *map, int type, char *text)
{
    hash_node_t *node = hashFind(map, text);

    if(node != 0)
    {
        return node;
    }

    node = calloc(1, sizeof(hash_node_t));
    node->text = calloc(strlen(text) + 1, sizeof(char));
	  node->type = type;
    node->nature = 0;
    strcpy(node->text, text);

    int hash = hashString(text);

    hash_node_t *curr = map->nodes[hash];
    if(curr == NULL)
    {
        map->nodes[hash] = node;
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

hash_node_t *hashFind(hash_map_t *map, char *text)
{
    int hash = hashString(text);

    hash_node_t *curr = map->nodes[hash];
    while(curr != NULL)
    {
        if(strcmp(curr->text, text) == 0)
            return curr;
	curr = curr->next;
    }
    return 0;
}

void hashPrint(hash_map_t *map)
{
    int i;
    for(i = 0; i < TABLE_SIZE; i++)
    {
        if(map->nodes[i] != NULL)
        {
            printf("Row:%d\n", i);
            printRow(map->nodes[i]);
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
