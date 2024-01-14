#include "errors.h"

#include <assert.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 10

// static const char* error_descriptions[] = {
//     [ERROR_MALLOC] = "Failed to allocate memory",
//     [ERROR_REALLOC] = "Failed to reallocate memory",
//     [ERROR_EMPTY_PROGRAM] = "Program is empty",
//     [ERROR_NIL] = "No error found",
//     [ERROR_APPEND] = "Failed to append",
//     [ERROR_UNBALANCED_PARENS] = "No closing parenthesis found",
//     [ERROR_EXPECTED_LPAREN] = "Expected an opening parenthesis",
// };

Errors* errors_init(void) {
  Errors* errs = malloc(sizeof(Errors));
  if (errs == NULL) {
    return NULL;
  }

  Error* data = malloc(sizeof(Error) * DEFAULT_CAPACITY);
  if (data == NULL) {
    return NULL;
  }

  errs->data = data;
  errs->capacity = DEFAULT_CAPACITY;
  errs->length = 0;

  return errs;
}

bool errors_append(Errors* errs, Error err) {
  assert(errs != NULL);

  // Request more memory if capacity is exceeded
  if (errs->length >= errs->capacity) {
    uint64_t new_cap = (errs->capacity + DEFAULT_CAPACITY);
    Error* tmp = realloc(errs->data, sizeof(Error) * new_cap);
    if (tmp == NULL) {
      return false;
    }
    errs->data = tmp;
    errs->capacity = new_cap;
  }

  // Append
  errs->data[errs->length++] = err;
  return true;
}
