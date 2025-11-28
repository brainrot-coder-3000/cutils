#pragma once

typedef unsigned int uint;

typedef struct Array {
    char *data;
    uint len;
    uint allocated;
    uint element_size;
} Array;

Array* array_new(uint element_size);
void array_free(Array *array, int free_segment);
Array* array_append_vals(Array *array, const void *data, uint len);
Array* array_prepend_vals(Array *array, const void *data, uint len);
Array* array_insert_vals(Array *array, uint index, const void *data, uint len);

#define array_append_val(a,v)   array_append_vals(a, &(v), 1)
#define array_prepend_val(a,v)  array_prepend_vals(a, &(v), 1)
#define array_insert_val(a,i,v) array_insert_vals(a, i, &(v), 1)
#define array_index(a,t,i)      (((t*)(void*)(a)->data)[(i)])
