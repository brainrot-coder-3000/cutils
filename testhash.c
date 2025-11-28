#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "hashtable.h"
#include <assert.h>


void test_string_hashtable() {
    HashTable *table = hashtable_new(str_hash, str_equal);
    assert(table != NULL);

    // Insert
    assert(hashtable_insert(table, "apple", "fruit"));
    assert(hashtable_insert(table, "carrot", "vegetable"));
    assert(hashtable_insert(table, "banana", "fruit"));
    assert(hashtable_size(table) == 3);

    // Lookup
    assert(strcmp((char*)hashtable_lookup(table, "apple"), "fruit") == 0);
    assert(strcmp((char*)hashtable_lookup(table, "carrot"), "vegetable") == 0);
    assert(strcmp((char*)hashtable_lookup(table, "banana"), "fruit") == 0);
    assert(hashtable_lookup(table, "orange") == NULL);

    // Contains
    assert(hashtable_contains(table, "apple"));
    assert(!hashtable_contains(table, "orange"));

    // Remove
    assert(hashtable_remove(table, "carrot"));
    assert(!hashtable_contains(table, "carrot"));
    assert(hashtable_size(table) == 2);

    hashtable_destroy(table);
}


void test_int_hashtable() {
    HashTable *table = hashtable_new(int_hash, int_equal);
    assert(table != NULL);

    int keys[] = {10, 20, 30, 40};
    char *values[] = {"ten", "twenty", "thirty", "forty"};

    // Insert
    for (int i = 0; i < 4; i++) {
        assert(hashtable_insert(table, &keys[i], values[i]));
    }
    assert(hashtable_size(table) == 4);

    // Lookup
    for (int i = 0; i < 4; i++) {
        assert(strcmp((char*)hashtable_lookup(table, &keys[i]), values[i]) == 0);
    }

    // Remove
    assert(hashtable_remove(table, &keys[2])); // remove 30
    assert(!hashtable_contains(table, &keys[2]));
    assert(hashtable_size(table) == 3);

    hashtable_destroy(table);
}

int main()
{
    Array *arr = array_new(sizeof(int));

    int x = 10, y = 20, z = 30;
    array_append_val(arr, x);
    array_append_val(arr, y);
    array_append_val(arr, z);

    int w = 15;
    array_insert_val(arr, 1, w);

    int v = 5;
    array_prepend_val(arr, v);

    printf("Array contents:\n");
    for (uint i = 0; i < arr->len; i++)
    {
        int value = array_index(arr, int, i);
        printf("%d ", value);
    }
    printf("\n");

    array_free(arr, 1);

    test_string_hashtable();
    test_int_hashtable();

    

    return 0;

}