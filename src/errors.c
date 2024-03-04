#include "errors.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

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

bool errors_cmp(Error* err1, Error* err2) {
  return strcmp(err1->file_name, err2->file_name) == 0 &&
         err1->type == err2->type && err1->row == err2->row &&
         err1->col == err2->col && err1->code_start == err2->code_start &&
         err1->code_end == err2->code_end;
}
