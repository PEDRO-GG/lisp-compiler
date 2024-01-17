#include "errors.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 10

Errors* errors_init(void) {
  Errors* errs = malloc(sizeof(Errors));
  if (errs == NULL) {
    fprintf(stderr, "errors_init(): malloc failed");
    exit(EXIT_FAILURE);
  }

  Error* data = malloc(sizeof(Error) * DEFAULT_CAPACITY);
  if (data == NULL) {
    fprintf(stderr, "errors_init(): malloc failed");
    exit(EXIT_FAILURE);
  }

  errs->data = data;
  errs->capacity = DEFAULT_CAPACITY;
  errs->length = 0;

  return errs;
}

void errors_append(Errors* errs, Error err) {
  assert(errs != NULL);

  // Request more memory if capacity is exceeded
  if (errs->length >= errs->capacity) {
    uint64_t new_cap = (errs->capacity + DEFAULT_CAPACITY);
    Error* tmp = realloc(errs->data, sizeof(Error) * new_cap);
    if (tmp == NULL) {
      fprintf(stderr, "errors_append(): realloc failed");
      exit(EXIT_FAILURE);
    }
    errs->data = tmp;
    errs->capacity = new_cap;
  }

  // Append
  errs->data[errs->length++] = err;

  // Check for unrecoverable errors
  // if (err.type == ERROR_MALLOC || err.type == ERROR_REALLOC ||
  //     err.type == ERROR_EMPTY_PROGRAM) {
  //   fprintf(stderr, "errors_append(): unrecoverable error encountered");
  //   errors_print(errs);
  //   exit(EXIT_FAILURE);
  // }
}

void errors_print(Errors* errs) {
  static const char* error_descriptions[] = {
      [ERROR_MALLOC] = "Failed to allocate memory",
      [ERROR_REALLOC] = "Failed to reallocate memory",
      [ERROR_EMPTY_PROGRAM] = "Program is empty",
      [ERROR_NIL] = "No error found",
      [ERROR_APPEND] = "Failed to append",
      [ERROR_UNBALANCED_PARENS] = "No closing parenthesis found",
      [ERROR_EXPECTED_LPAREN] = "Expected an opening parenthesis",
  };

  for (uint64_t i = 0; i < errs->length; i++) {
    Error err = errs->data[i];
    printf("error[%d]: %s\n", err.type, error_descriptions[err.type]);
    if (err.file_name) {
      printf("--> %s:%llu:%llu\n", err.file_name, err.row, err.col);
    }
  }
}
