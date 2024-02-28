#ifndef ERRORS_H
#define ERRORS_H

#include <stdbool.h>
#include <stdint.h>

#include "array.h"

typedef enum {
  // Generic Errors
  ERROR_MALLOC,
  ERROR_REALLOC,
  ERROR_EMPTY_PROGRAM,
  ERROR_NIL,

  // Token Errors
  ERROR_APPEND,
  ERROR_UNBALANCED_PARENS,
  ERROR_EXPECTED_LPAREN,
  ERROR_EXPECTED_IDENTIFIER,
  ERROR_DUPLICATE_IDENT,
  ERROR_DIFF_TYPES,
} ErrorType;

/*
Below is an example of how errors will be printed to the console

 error[0123]: mismatched types
  --> ./src/filename.c:406:59
  |   (call foo args1 arg2)
  |             ^^^^^ expected `int`, got `bool`
  |
*/
typedef struct {
  ErrorType type;
  char* file_name;
  uint64_t row;
  uint64_t col;
  uint64_t code_start;
  uint64_t code_end;
} Error;

void errors_print(Array* errs);

#endif  // ERRORS_H
