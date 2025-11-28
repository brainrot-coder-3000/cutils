#pragma once
#include <stddef.h>

typedef unsigned int uint;
typedef uint (*HashFunc)(const void *key);
typedef int (*EqualFunc)(const void *a, const void *b);

#define INITIAL_SIZE 16
#define LOAD_FACTOR 0.75

typedef struct _HashNode {
    void *key;
    void *value;
    uint hash;
    struct _HashNode *next;
} HashNode;

typedef struct _HashTable {
    HashNode **buckets;
    uint num_buckets;
    uint num_items;
    HashFunc hash_func;
    EqualFunc key_equal_func;
} HashTable;

/* Core functions */
HashTable* hashtable_new(HashFunc hash_func, EqualFunc key_equal_func);
void hashtable_destroy(HashTable *table);
int hashtable_insert(HashTable *table, void *key, void *value);
int hashtable_remove(HashTable *table, const void *key);
void* hashtable_lookup(HashTable *table, const void *key);
int hashtable_contains(HashTable *table, const void *key);
uint hashtable_size(HashTable *table);

/* Common hash/equal functions */
uint str_hash(const void *v);
int str_equal(const void *v1, const void *v2);
uint int_hash(const void *v);
int int_equal(const void *v1, const void *v2);
uint direct_hash(const void *v);
int direct_equal(const void *v1, const void *v2);

