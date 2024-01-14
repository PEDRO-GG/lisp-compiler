#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>
#include <stdint.h>

#include "fatstr.h"
#include "token.h"

typedef enum {
  RESULT_BOOL,
  RESULT_NUM,
  RESULT_STRING,
  RESULT_NOOP,
} ResultType;

typedef struct {
  ResultType type;
  union {
    bool boolean;
    int64_t num;
    FatStr string;
  } value;
} Result;

typedef struct {
  FatStr name;
  Result result;
} Var;

typedef enum {
  SYMBOL_VAR,
  SYMBOL_FUNC,
} SymbolType;

typedef struct {
  SymbolType type;
  union {
    Var var;
    Token* func;
  } value;
} Symbol;

typedef struct Env {
  Symbol* data;
  uint64_t capacity;
  uint64_t length;
  struct Env* next;
} Env;

typedef enum {
  EVALUATE_ERROR_MALLOC,
  EVALUATE_ERROR_REALLOC,
  EVALUATE_ERROR_NULL_TOKEN,
  EVALUATE_ERROR_EMPTY_LIST,
  EVALUATE_ERROR_EXPECTED_OP,
  EVALUATE_ERROR_UNMATCHED_TYPES,
  EVALUATE_ERROR_EXPECTED_OPERAND,
  EVALUATE_ERROR_EXPECTED_IDENT,
  EVALUATE_ERROR_NO_SCOPE,
  EVALUATE_ERROR_DUPLICATE_IDENT,
  EVALUATE_ERROR_EXPECTED_BOOL,
  EVALUATE_ERROR_EXPECTED_LIST,
  EVALUATE_ERROR_BREAK,
  EVALUATE_ERROR_VAR_NOT_FOUND,
  EVALUATE_ERROR_FUNC_NOT_FOUND,
  EVALUATE_ERROR_ILLEGAL_TOKEN,
  EVALUATE_ERROR_NIL,
} EvaluateError;

EvaluateError evaluate(Token* token, Env* env, Result* result);
bool rescmp(const Result* r1, const Result* r2);
bool result_to_string(const Result* r, char* buffer);

Env* env_make(EvaluateError* err, Env* next);
EvaluateError env_append(Env* env, Symbol sym);
bool env_contains_var(Env* env, FatStr* str);
Var* env_find_var(Env* env, FatStr* str);
Token* env_find_func(Env* env, FatStr* str);

#endif  // INTERPRETER_H
