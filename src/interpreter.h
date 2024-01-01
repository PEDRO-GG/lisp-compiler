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

typedef struct Env {
  Var* data;
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
  EVALUATE_ERROR_ILLEGAL_TOKEN,
  EVALUATE_ERROR_NIL,
} EvaluateError;

EvaluateError evalute(Token* token, Result* result);
bool rescmp(const Result* r1, const Result* r2);

Env* env_make(EvaluateError* err, Env* next);
EvaluateError env_append(Env* env, Var var);

#endif  // INTERPRETER_H
