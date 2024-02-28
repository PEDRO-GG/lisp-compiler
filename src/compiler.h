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
  TYPE_VOID,
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
  uint64_t stack;  // next available register
} Compiler;

typedef struct {
  ValueType type;
  uint64_t reg;
} CompileResult;

Compiler new_compiler(void);
CompileResult compile(Compiler* cs, Token* token);
void enter_scope(Compiler* cs);
void leave_scope(Compiler* cs);
bool is_defined(Compiler* cs, FatStr* str);
uint64_t tmp(Compiler* cs);

#endif  // COMPILER_H
