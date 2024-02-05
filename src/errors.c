#include "errors.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void errors_print(Array* errs) {
  static const char* error_descriptions[] = {
      [ERROR_MALLOC] = "Failed to allocate memory",
      [ERROR_REALLOC] = "Failed to reallocate memory",
      [ERROR_EMPTY_PROGRAM] = "Program is empty",
      [ERROR_NIL] = "No error found",
      [ERROR_APPEND] = "Failed to append",
      [ERROR_UNBALANCED_PARENS] = "No closing parenthesis found",
      [ERROR_EXPECTED_LPAREN] = "Expected an opening parenthesis",
  };

  for (size_t i = 0; i < array_length(errs); i++) {
    Error* err = (Error*)array_get(errs, i);
    printf("error[%d]: %s\n", err->type, error_descriptions[err->type]);
    if (err->file_name) {
      printf("--> %s:%llu:%llu\n", err->file_name, err->row, err->col);
    }
  }
}
