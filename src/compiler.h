#ifndef COMPILER_H
#define COMPILER_H
#include <stdint.h>

#include "errors.h"
#include "fatstr.h"
#include "token.h"

typedef enum {
  LOCAL_BOOL,
  LOCAL_INT,
  LOCAL_STR,
  LOCAL_VOID,
} LocalType;

typedef struct {
  FatStr name;
  LocalType type;
} Local;

typedef struct Scope {
  struct Scope* parent;  // the parent scope
  uint64_t length;       // number of local variables seen
  uint64_t capacity;     // number of locals allocated
  Local* locals;         // array of locals
} Scope;

typedef struct {
  Scope* scope;  // the very first scope
  Errors* errs;  // compiler errors
} CompilerState;

void compile(CompilerState* cs, Token* token);
void enter_scope(CompilerState* cs);
void leave_scope(CompilerState* cs);

#endif  // COMPILER_H
