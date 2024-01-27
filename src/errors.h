#ifndef ERRORS_H
#define ERRORS_H

#include <stdbool.h>
#include <stdint.h>

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

typedef struct {
  Error* data;
  uint64_t capacity;
  uint64_t length;
} Errors;

Errors* errors_init(void);
void errors_append(Errors* errs, Error err);
void errors_append_fatal(Errors* errs, Error err);
void errors_print(Errors* errs);

#endif  // ERRORS_H
