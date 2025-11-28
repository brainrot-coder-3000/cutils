#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE 16
#define LOAD_FACTOR 0.75

static void hashtable_resize(HashTable *table) {
    uint old_size = table->num_buckets;
    uint new_size = old_size * 2;
    HashNode **new_buckets = calloc(new_size, sizeof(HashNode*));
    
    if (!new_buckets) return;
    
    /* Rehash all existing nodes */
    for (uint i = 0; i < old_size; i++) {
        HashNode *node = table->buckets[i];
        while (node) {
            HashNode *next = node->next;
            uint bucket = node->hash % new_size;
            node->next = new_buckets[bucket];
            new_buckets[bucket] = node;
            node = next;
        }
    }
    
    free(table->buckets);
    table->buckets = new_buckets;
    table->num_buckets = new_size;
}

HashTable* hashtable_new(HashFunc hash_func, EqualFunc key_equal_func) {
    HashTable *table = malloc(sizeof(HashTable));
    if (!table) return NULL;
    
    table->buckets = calloc(INITIAL_SIZE, sizeof(HashNode*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    
    table->num_buckets = INITIAL_SIZE;
    table->num_items = 0;
    table->hash_func = hash_func;
    table->key_equal_func = key_equal_func;
    
    return table;
}

void hashtable_destroy(HashTable *table) {
    if (!table) return;
    
    for (uint i = 0; i < table->num_buckets; i++) {
        HashNode *node = table->buckets[i];
        while (node) {
            HashNode *next = node->next;
            free(node);
            node = next;
        }
    }
    
    free(table->buckets);
    free(table);
}

int hashtable_insert(HashTable *table, void *key, void *value) {
    if (!table) return 0;
    
    uint hash = table->hash_func(key);
    uint bucket = hash % table->num_buckets;
    
    /* Check if key already exists */
    HashNode *node = table->buckets[bucket];
    while (node) {
        if (node->hash == hash && table->key_equal_func(node->key, key)) {
            node->value = value;
            return 1;
        }
        node = node->next;
    }
    
    /* Create new node */
    HashNode *new_node = malloc(sizeof(HashNode));
    if (!new_node) return 0;
    
    new_node->key = key;
    new_node->value = value;
    new_node->hash = hash;
    new_node->next = table->buckets[bucket];
    table->buckets[bucket] = new_node;
    table->num_items++;
    
    /* Resize if needed */
    if ((double)table->num_items / table->num_buckets > LOAD_FACTOR) {
        hashtable_resize(table);
    }
    
    return 1;
}

int hashtable_remove(HashTable *table, const void *key) {
    if (!table) return 0;
    
    uint hash = table->hash_func(key);
    uint bucket = hash % table->num_buckets;
    
    HashNode **node_ptr = &table->buckets[bucket];
    while (*node_ptr) {
        HashNode *node = *node_ptr;
        if (node->hash == hash && table->key_equal_func(node->key, key)) {
            *node_ptr = node->next;
            free(node);
            table->num_items--;
            return 1;
        }
        node_ptr = &node->next;
    }
    
    return 0;
}

void* hashtable_lookup(HashTable *table, const void *key) {
    if (!table) return NULL;
    
    uint hash = table->hash_func(key);
    uint bucket = hash % table->num_buckets;
    
    HashNode *node = table->buckets[bucket];
    while (node) {
        if (node->hash == hash && table->key_equal_func(node->key, key)) {
            return node->value;
        }
        node = node->next;
    }
    
    return NULL;
}

int hashtable_contains(HashTable *table, const void *key) {
    return hashtable_lookup(table, key) != NULL;
}

uint hashtable_size(HashTable *table) {
    return table ? table->num_items : 0;
}

/* Common hash functions */

uint str_hash(const void *v) {
    const char *str = v;
    uint hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    
    return hash;
}

int str_equal(const void *v1, const void *v2) {
    return strcmp(v1, v2) == 0;
}

uint int_hash(const void *v) {
    return *(const int*)v;
}

int int_equal(const void *v1, const void *v2) {
    return *(const int*)v1 == *(const int*)v2;
}

uint direct_hash(const void *v) {
    return (uint)(size_t)v;
}

int direct_equal(const void *v1, const void *v2) {
    return v1 == v2;
}
