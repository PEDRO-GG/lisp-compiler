#include "array.h"

#include <stdlib.h>
#include <string.h>

struct Array {
  size_t length;
  size_t capacity;
  size_t element_size;
  void* data;
};

Array* array_new(size_t initial_capacity, size_t element_size) {
  // Allocate memory for the Array structure
  Array* array = (Array*)malloc(sizeof(Array));
  if (array == NULL) {
    // Handle memory allocation failure
    return NULL;
  }

  // Set initial length to 0 as the array is empty
  array->length = 0;

  // Set the initial capacity
  array->capacity = initial_capacity;

  // Set the size of each element
  array->element_size = element_size;

  // Allocate memory for the data
  array->data = malloc(initial_capacity * element_size);
  if (array->data == NULL) {
    // Handle memory allocation failure
    free(array);
    return NULL;
  }

  return array;
}

// Function to append an element to the array
int array_append(Array* array, void* element) {
  if (array == NULL || element == NULL) {
    return -1;  // Error: Invalid input
  }

  // Check if the array needs to be resized
  if (array->length == array->capacity) {
    size_t new_capacity = array->capacity * 2;  // Double the capacity
    void* new_data = realloc(array->data, new_capacity * array->element_size);
    if (new_data == NULL) {
      return -1;  // Error: Memory allocation failed
    }
    array->data = new_data;
    array->capacity = new_capacity;
  }

  // Copy the new element to the end of the array
  void* destination =
      (char*)array->data + (array->length * array->element_size);
  memcpy(destination, element, array->element_size);
  array->length++;

  return 0;  // Success
}

// Function to get an element
void* array_get(Array* array, size_t index) {
  if (array == NULL || index >= array->length) {
    return NULL;  // Error: Invalid input or index out of bounds
  }

  // Calculate the address of the desired element
  char* element = (char*)array->data + (index * array->element_size);
  return (void*)element;
}

// Function to remove an element from the array
int array_remove(Array* array, size_t index) {
  if (array == NULL || index >= array->length) {
    return -1;  // Error: Invalid input
  }

  // Calculate the address of the element to remove
  void* element_to_remove = (char*)array->data + (index * array->element_size);

  // Shift the elements after the removed element
  if (index < array->length - 1) {
    void* destination = element_to_remove;
    void* source = (char*)element_to_remove + array->element_size;
    size_t bytes_to_move = (array->length - index - 1) * array->element_size;
    memmove(destination, source, bytes_to_move);
  }

  // Decrease the array length
  array->length--;

  return 0;  // Success
}

// Pops an element
void* array_pop(Array* array) {
  if (array == NULL || array->length == 0) {
    return NULL;  // Error: Invalid input or array is empty
  }

  // Calculate the address of the last element
  array->length--;
  char* element = (char*)array->data + (array->length * array->element_size);
  return (void*)element;
}

// Function to remove all elements from the nth index onwards
int array_truncate(Array* array, size_t n) {
  if (array == NULL) {
    return -1;  // Error: Invalid input
  }

  if (n < array->length) {
    array->length = n;
  }

  return 0;  // Success
}

// Returns the length
size_t array_length(Array* array) { return array->length; }

// Function to append a string to the array (without null terminator)
int array_append_str(Array* array, const char* str) {
  if (array == NULL || str == NULL) {
    return -1;  // Error: Invalid input
  }

  // Append each character of the string to the array
  for (size_t i = 0; str[i] != '\0'; ++i) {
    if (array_append(array, (void*)&str[i]) != 0) {
      return -1;  // Error: Failed to append character
    }
  }

  return 0;  // Success
}
