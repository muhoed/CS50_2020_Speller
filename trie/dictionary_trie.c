// Implements a dictionary's functionality

#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "dictionary.h"

// Number of buckets in hash table
const unsigned int N = 27;

// array of path vector to reach the word in trie
unsigned int vectorPath[45];


// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *nextLevel[N];
}
node;

void delNode(struct node **del);

// Hash table
node *table[N];

// Number of words in dictionary
unsigned int dict_size = 0;

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    unsigned int hash_index = hash(word);
    if (table[hash_index] == NULL)
    {
        return false;
    }
    node *tmp = table[hash_index];
    int l = strlen(word);
    for (int i = 1; i < l; i++)
    {
        if (tmp->nextLevel[vectorPath[i]] == NULL)
        {
            return false;
        }
        tmp = tmp->nextLevel[vectorPath[i]];
    }
    if (!strcasecmp(word, tmp->word))
        {
            return true;
        }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // TODO
    // return 0 ... 25 for alphabetics, 26 for ¨ ´ ¨ and fill in an array which represents a path vector 
    int l = strlen(word);
    for (int i = 0; i < l; i++)
    {
       if (tolower(word[i]) == 39)
       {
           vectorPath[i] = 26;
       }
       else
       {
       vectorPath[i] = tolower(word[i]) - 97;
       }
    }
    return vectorPath[0];
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
    int l = strlen(word);
    // create path
    //first create head
    node **tmp = &table[hash_index];
    for (int i = 0; i < l; i++)
    {
        if (*tmp == NULL)
        {
            // create level
            node *n = malloc(sizeof(node));
            if (n == NULL)
            {
                return false;
            }
            for (int a = 0; a < N; a++)
            {
                n->nextLevel[a] = NULL;
            }
            // put at this level if true
            if (l == i + 1)
            {
                strcpy(n->word, word);
                *tmp = n;
                return true;
            }
            strcpy(n->word, "");
            *tmp = n;
            // move to next level
            tmp = &(*tmp)->nextLevel[vectorPath[i + 1]];
        }
        // if level already exists
        else
        {
            // put at this level if true
            if (l == i + 1)
            {
                // put at this level
                strcpy((*tmp)->word, word);
                return true;
            }
            // move to next level
            tmp = &(*tmp)->nextLevel[vectorPath[i + 1]];
        }
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
        if (table[i] != NULL)
        {
            delNode(&table[i]);
        }
    }
    return true;
}

void delNode(struct node **del)
{
    if (*del == NULL)
    {
        return;
    }
    for (int i = 0; i < N; i++)
    {
        if ((*del)->nextLevel[i] != NULL)
        {
            delNode(&(*del)->nextLevel[i]);
        }
    }
    free(*del);
    return;
}