// Implements a dictionary's functionality

#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "dictionary.h"

// Represents a node in a list
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// represents a node in a hash table which is simulteneously a header in a list
typedef struct header
{
    struct node *list[26];
}
header;

// Number of buckets in hash table
const unsigned int N = 262144; // 1048576; // 524288; // 262144;

// Hash table
header *table[N];

// Number of words in dictionary
unsigned int dict_size = 0;

void delLists(struct header **head);

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    unsigned int hash_index = hash(word);
    int num = tolower(word[0]) - 97;
    if (table[hash_index] == NULL)
    {
        return false;
    }
    if (table[hash_index]->list[num] == NULL)
    {
        return false;
    }
    if (!strcasecmp(word, table[hash_index]->list[num]->word))
    {
        return true;
    }
    for (node *tmp = table[hash_index]->list[num]->next; tmp != NULL; tmp = tmp->next)
    {
        if (!strcasecmp(word, tmp->word))
        {
            return true;
        }
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // TODO

    // sdbm algorithm based on description derived from cse.gorku.ca/~oz/hash
    // unsigned int hash = 5381;
    // char c;
    // int l = strlen(word);
    // for (int i = 0; i < l; i++)
    // {
    //    c = tolower(word[i]);
    //    hash = c + (hash << 6) + (hash << 16) - hash;
    // }

    // fnv-1a algorithm based on Wikipedia and Ian Boyd´s answer here https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed/145633#145633
    unsigned int prime = 16777619;
	unsigned int hash = 2166136261;
	char c;
	int l = strlen(word);
	for (int i = 0; i < l; i++)
	{
		c = tolower(word[i]);
		hash ^= c;
		hash *= prime;
	}

    return hash % N;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // TODO
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }
    // put each word from dictionary into hash table
    char *wrd = malloc(LENGTH + 1);
    while (fscanf(dict, "%s", wrd) != EOF)
    {
        // get hash index
        unsigned int hash_index = hash(wrd);
        // insert to hash table
        if (insert(hash_index, wrd))
        {
            // append words counter
            dict_size++;
        }
        else
        {
            fclose(dict);
            free(wrd);
            return false;
        }
    }
    free(wrd);
    fclose(dict);
    return true;
}

// insert word into hash table using sorted list when collide
bool insert(const int hash_index, const char *word)
{
    int num = tolower(word[0]) - 97;
    if (table[hash_index] == NULL)
    {
        // create header element
        header *nhead = malloc(sizeof(header));
        if (nhead == NULL)
        {
            return false;
        }
        for (int i = 0; i < 26; i++)
        {
            nhead->list[i] = NULL;
        }
        table[hash_index] = nhead;
    }
    // create new list if list does not exists
    if (table[hash_index]->list[num] == NULL)
    {
        node *nnode = malloc(sizeof(node));
        if (nnode == NULL)
        {
            return false;
        }
        strcpy(nnode->word, word);
        nnode->next = NULL;
        table[hash_index]->list[num] = nnode;
        return true;
    }
    // insert it in list if list exists
    node *nnode = malloc(sizeof(node));
    if (nnode == NULL)
    {
        return false;
    }
    strcpy(nnode->word, word);
    nnode->next = table[hash_index]->list[num];
    table[hash_index]->list[num] = nnode;
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    return dict_size;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // TODO
    for (int i = 0; i < N; i++)
    {
        if (table[i] != NULL)
        {
            delLists(&table[i]);
            free(table[i]);
        }
    }
    return true;
}

void delLists(struct header **head)
{
    for (int i = 0; i < 26; i++)
    {
        for (node *ntmp = (*head)->list[i]; ntmp != NULL; ntmp = (*head)->list[i])
        {
            (*head)->list[i] = ntmp->next;
            free(ntmp);
        }
    }
}