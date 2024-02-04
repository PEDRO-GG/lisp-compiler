#include "array.h"

#include <stdio.h>
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

// Function to append a formatted string to the array
int array_append_fmt(Array* array, const char* format, ...) {
  if (array == NULL || format == NULL) {
    return -1;  // Error: Invalid input
  }

  // Start variadic arguments
  va_list args;
  va_start(args, format);

  // Determine the length of the formatted string
  int length = vsnprintf(NULL, 0, format, args);
  if (length < 0) {
    va_end(args);
    return -1;  // Error: Formatting failed
  }

  // Allocate a temporary buffer for the formatted string
  char* buffer = (char*)malloc(length + 1);  // +1 for null terminator
  if (buffer == NULL) {
    va_end(args);
    return -1;  // Error: Memory allocation failed
  }

  // Actually format the string
  vsnprintf(buffer, length + 1, format, args);

  // Append the formatted string to the array (excluding the null terminator)
  for (int i = 0; i < length; ++i) {
    if (array_append(array, &buffer[i]) != 0) {
      free(buffer);
      va_end(args);
      return -1;  // Error: Failed to append character
    }
  }

  // Clean up
  free(buffer);
  va_end(args);

  return 0;  // Success
}

// Function to compare array content with a string
bool array_compare_with_string(Array* array, const char* str) {
  if (array->length != strlen(str)) {
    return false;  // Length mismatch
  }

  for (size_t i = 0; i < array->length; ++i) {
    char* array_char = (char*)array_get(array, i);
    if (*array_char != str[i]) {
      return false;  // Character mismatch
    }
  }

  return true;  // Match
}
