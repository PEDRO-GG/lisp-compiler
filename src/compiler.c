#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void compile_var(Compiler* cs, Token* token) {
  Token* var_name = token->value.list.data[1];
  if (var_name->type != TOKEN_IDENTIFIER) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_EXPECTED_IDENTIFIER,
                           });
  }

  if (is_defined(cs, &var_name->value.identifier)) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_DUPLICATE_IDENT,
                           });
  }

  compile(cs, token->value.list.data[2]);

  if (array_append(cs->idents, &(Identifier){
                                   .name = var_name->value.identifier,
                                   .type = TYPE_INT,
                               }) != 0) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_MALLOC,
                           });
  }
}

void compile_do(Compiler* cs, Token* token) {
  enter_scope(cs);

  for (uint64_t i = 1; i < token->value.list.length; i++) {
    compile(cs, token->value.list.data[i]);
  }

  leave_scope(cs);
}

void compile_list(Compiler* cs, Token* token) {
  uint64_t length = token->value.list.length;

  // Example: (var a 1)
  if (token->value.list.data[0]->type == TOKEN_VAR && length == 3) {
    compile_var(cs, token);
  }

  // Example: (do ...)
  if (token->value.list.data[0]->type == TOKEN_DO && length > 1) {
    compile_do(cs, token);
  }
}

Compiler new_compiler(void) {
  Compiler compiler = (Compiler){
      .idents = array_new(10, sizeof(Identifier)),
      .scopes = array_new(10, sizeof(uint64_t)),
      .code = array_new(10, sizeof(char)),
      .errs = array_new(10, sizeof(Error)),
      .stack = 0,
  };

  return compiler;
}

void compile(Compiler* cs, Token* token) {
  if (token == NULL) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_EMPTY_PROGRAM,
                           });
  }

  switch (token->type) {
    case TOKEN_LIST: {
      compile_list(cs, token);
      break;
    }
    case TOKEN_NUM: {
      array_append_fmt(cs->code, "const %lld %lld", token->value.num,
                       cs->stack++);
      break;
    }

    default:
      break;
  }
}

void enter_scope(Compiler* cs) {
  size_t length = array_length(cs->idents);
  if (array_append(cs->scopes, &length) != 0) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_MALLOC,
                           });
  }
}

void leave_scope(Compiler* cs) {
  size_t* current_scope = (size_t*)array_pop(cs->scopes);
  array_truncate(cs->idents, *current_scope);
}

bool is_defined(Compiler* cs, FatStr* str) {
  for (size_t i = 0; i < array_length(cs->idents); i++) {
    Identifier* cur = array_get(cs->idents, i);
    if (fatstr_cmp(&cur->name, str)) {
      return true;
    }
  }

  return false;
}
