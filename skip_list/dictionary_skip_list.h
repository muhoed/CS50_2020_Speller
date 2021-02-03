// Declares a dictionary's functionality

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// Maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

// for FNV1A-Pippip-Yurii hash function
//#define _PADr_KAZE(x, n) ( ((x) << (n))>>(n) )

// Prototypes
bool check(const char *word);
unsigned int hash(const char *word);
unsigned int rehash(const char *word);
bool load(const char *dictionary);
unsigned int size(void);
bool unload(void);
bool insert(const int hash_index, const char *word);

#endif // DICTIONARY_H
