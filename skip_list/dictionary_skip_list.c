// Implements a dictionary's functionality

#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    int key;
    char word[LENGTH + 1];
    struct node *forward[2];
}
node;

//typedef struct skiplist
//{
//    int level;
//    struct node *header;
//}
//skiplist;

bool initSkiplist(const int hash_index);
bool snode(const int hash_index, const char *word, int o);

// Number of buckets in hash table
const unsigned int N = 65536; // 1048576; // 524288; // 262144;

// Hash table
node *table[N];

// Number of words in dictionary
unsigned int dict_size = 0;

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    //int len = strlen(word);
    //char *wrd = malloc(LENGTH +1);
    //for (int i = 0; i < len; i++)
    //{
    //    wrd[i] = tolower(word[i]);
    //}
    unsigned int hash_index = hash(word);
    if (table[hash_index] == NULL)
    {
        return false;
    }
    if (snode(hash_index, word, 1))
    {
        return true;
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
		if (word[i] > 64 && word[i] < 91)
		{
		    c = word[i] + 32; //tolower(word[i]);
		}
		else
		{
		    c = word[i];
		}
		hash ^= c;
		hash *= prime;
	}
	
	// FNV1A-Pippip-Yurii from here https://www.codeproject.com/Articles/716530/Fastest-Hash-Function-for-Table-Lookups-in-C
	//const unsigned int PRIME = 591798841; unsigned int hash32; unsigned long long hash64 = 14695981039346656037U;
	//size_t Cycles, NDhead;
	// unsigned int hash;
	//int wrdlen = strlen(word);
    //if (wrdlen > 8) 
    //{
	//    Cycles = ((wrdlen - 1)>>4) + 1; 
	//    NDhead = wrdlen - (Cycles<<3);
//#pragma nounroll
    //    for (; Cycles--; word += 8)
    //    {
	//	    hash64 = ( hash64 ^ (*(uint64_t *)(word)) ) * PRIME;        
	//	    hash64 = ( hash64 ^ (*(uint64_t *)(word+NDhead)) ) * PRIME;        
	//    }
    //} 
    //else
    //{
	//    hash64 = ( hash64 ^ _PADr_KAZE(*(uint64_t *)(word+0), (8-wrdlen)<<3) ) * PRIME; 
    //}
    //    hash32 = (uint32_t)(hash64 ^ (hash64>>32)); 
    //    hash = hash32 ^ (hash32 >> 16);

	
	// murmur3 algorithm from this thread https://stackoverflow.com/questions/32795453/use-of-murmurhash-in-c
	
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
    if (table[hash_index] == NULL)
    {
        // initialize new skip list
        if (!initSkiplist(hash_index))
        {
            return false;
        }
    }
     // insert new node
     if (snode(hash_index, word, 2))
     {
         return true;
     }
    return false;
}

bool initSkiplist(const int hash_index)
{
    node **tmp = &table[hash_index];
    // create first node limiter
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        return false;
    }
    *tmp = n;
    n->key = 96;
    strcpy (n->word, "");
    // create last node limiter
    node *x = malloc(sizeof(node));
    if (x == NULL)
    {
        return false;
    }
    x->key = 123;
    strcpy (x->word, "");
    for (int i = 0; i < 2; i++)
    {
        n->forward[i] = x;
        x->forward[i] = NULL;
    }
    return true;
}

bool snode(const int hash_index, const char *word, int o)
{
    int key = (int) tolower(word[0]);
    node *x = table[hash_index];
    node **tmp[2];
    if (x->key == key && !strcasecmp(word, x->word))
    {
        return true;
    }
    for (int i = 1; i >= 0; i--)
    {
        while(x->forward[i]->key <= key)
        {
            if (x->forward[i]->key == key && !strcasecmp(word, x->forward[i]->word))
            {
                return true;
            }
            x = x->forward[i];
        }
        tmp[i] = &x;
    }
    if (o == 1)
    {
        return false;
    }   
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        return false;
    }
    n->key = key;
    strcpy(n->word, word);
    n->forward[0] = (*tmp[0])->forward[0];
    (*tmp[0])->forward[0] = n;
    if ((*tmp[1])->key != n->key && !((n->key - 97) % 5))
    {
        n->forward[1] = (*tmp[1])->forward[1];
        (*tmp[1])->forward[1] = n;
    }
    else
    {
        n->forward[1] = NULL;
    }
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
        for (node *tmp = table[i]; tmp != NULL; tmp = table[i])
        {
            table[i] = tmp->forward[0];
            free(tmp);
        
        }
    }
    return true;
}
