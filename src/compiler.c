#include "compiler.h"

#include <stdlib.h>

void compile_do(CompilerState* cs, Token* token) {
  enter_scope(cs);

  for (uint64_t i = 1; i < token->value.list.length; i++) {
    compile(cs, token->value.list.data[i]);
  }

  leave_scope(cs);
}

void compile_list(CompilerState* cs, Token* token) {
  uint64_t length = token->value.list.length;

  // Example: (do ...)
  if (token->value.list.data[0]->type == TOKEN_DO && length > 1) {
    compile_do(cs, token);
  }
}

void compile(CompilerState* cs, Token* token) {
  if (token == NULL) {
    errors_append_fatal(cs->errs, (Error){
                                      .type = ERROR_EMPTY_PROGRAM,
                                  });
  }

  switch (token->type) {
    case TOKEN_LIST: {
      compile_list(cs, token);
      break;
    }

    default:
      break;
  }
}

void enter_scope(CompilerState* cs) {
  Scope* new_scope = malloc(sizeof(Scope));
  if (new_scope == NULL) {
    errors_append_fatal(cs->errs, (Error){
                                      .type = ERROR_MALLOC,
                                  });
    return;
  }

  new_scope->parent = cs->scope;
  new_scope->capacity = 10;
  new_scope->length = 0;

  new_scope->locals = malloc(sizeof(Local) * new_scope->capacity);
  if (new_scope->locals == NULL) {
    errors_append_fatal(cs->errs, (Error){
                                      .type = ERROR_MALLOC,
                                  });
    return;
  }

  return;
}

void leave_scope(CompilerState* cs) {
  Scope* parent = cs->scope->parent;

  free(cs->scope->locals);
  free(cs->scope);

  cs->scope = parent;
}
