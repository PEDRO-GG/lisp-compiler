#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>
#include <stdint.h>

typedef struct Array Array;

Array* array_new(size_t initial_capacity, size_t element_size);
int array_append(Array* array, void* element);
void* array_get(Array* array, size_t index);
int array_remove(Array* array, size_t index);
void* array_pop(Array* array);
int array_truncate(Array* array, size_t n);

#endif  // ARRAY_H
