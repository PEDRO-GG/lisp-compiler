#ifndef COMPILER_H
#define COMPILER_H
#include <stdint.h>

#include "array.h"
#include "errors.h"
#include "fatstr.h"
#include "token.h"

typedef enum {
  TYPE_INT,
  TYPE_FUNC,
  TYPE_BYTE,
  TYPE_BYTE_PTR,
} ValueType;

typedef struct {
  FatStr name;
  ValueType type;
} Identifier;

typedef struct {
  Array* idents;   // array of identifiers
  Array* scopes;   // array of indices from idents
  Array* code;     // array of chars
  Array* errs;     // array of errors
  Token* token;    // token to compile
  uint64_t stack;  // number of variables on the stack
} Compiler;

Compiler new_compiler(Token* token);
void compile(Compiler* cs);
void enter_scope(Compiler* cs);
void leave_scope(Compiler* cs);

#endif  // COMPILER_H
