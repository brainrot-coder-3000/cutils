#include "array.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned int uint;

static inline int array_maybe_resize(Array *array, uint new_len) {
    uint new_size = new_len * array->element_size;
    
    if (new_size <= array->allocated) {
        return 1;
    }
    
    uint new_allocated = array->allocated == 0 ? 16 * array->element_size : array->allocated * 2;
    while (new_allocated < new_size) {
        new_allocated *= 2;
    }
    
    char *new_data = realloc(array->data, new_allocated);
    if (!new_data) return 0;
    
    array->data = new_data;
    array->allocated = new_allocated;
    
    return 1;
}

Array* array_new(uint element_size) {
    Array *array = malloc(sizeof(Array));
    
    array->data = NULL;
    array->len = 0;
    array->allocated = 0;
    array->element_size = element_size;
    
    return array;
}

void array_free(Array *array, int free_segment) {
    if (free_segment && array->data) {
        free(array->data);
    }
    free(array);
}

Array* array_append_vals(Array *array, const void *data, uint len) {
    uint new_len = array->len + len;
    
    if (!array_maybe_resize(array, new_len)) {
        return array;
    }
    
    memcpy(array->data + array->len * array->element_size, data, len * array->element_size);
    array->len = new_len;
    
    return array;
}

Array* array_prepend_vals(Array *array, const void *data, uint len) {
    if (len == 0) return array;
    
    uint new_len = array->len + len;
    
    if (!array_maybe_resize(array, new_len)) {
        return array;
    }
    
    memmove(array->data + len * array->element_size,
            array->data,
            array->len * array->element_size);
    
    memcpy(array->data, data, len * array->element_size);
    array->len = new_len;
    
    return array;
}

Array* array_insert_vals(Array *array, uint index, const void *data, uint len) {
    if (index >= array->len) return array_append_vals(array, data, len);
    
    uint new_len = array->len + len;
    
    if (!array_maybe_resize(array, new_len)) {
        return array;
    }
    
    uint insert_offset = index * array->element_size;
    uint move_size = (array->len - index) * array->element_size;
    
    memmove(array->data + insert_offset + len * array->element_size,
            array->data + insert_offset,
            move_size);
    
    memcpy(array->data + insert_offset, data, len * array->element_size);
    array->len = new_len;
    
    return array;
}