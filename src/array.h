#ifndef ARRAY_H
#define ARRAY_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct Array Array;

Array* array_new(size_t initial_capacity, size_t element_size);
int array_append(Array* array, void* element);
void* array_get(Array* array, size_t index);
int array_remove(Array* array, size_t index);
void* array_pop(Array* array);
int array_truncate(Array* array, size_t n);
size_t array_length(Array* array);
int array_append_str(Array* array, const char* str);
char* array_to_str(Array* array);
int array_append_fmt(Array* array, const char* format, ...);
bool array_compare_with_string(Array* array, const char* str);
bool array_cmp(Array* arr1, Array* arr2);
#endif  // ARRAY_H
