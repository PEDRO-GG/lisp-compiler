#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

CompileResult compile_var(Compiler* cs, Token* token) {
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

  if (array_append(cs->idents, &(Identifier){
                                   .name = var_name->value.identifier,
                                   .type = TYPE_INT,
                               }) != 0) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_MALLOC,
                           });
  }

  return compile(cs, token->value.list.data[2]);
}

CompileResult compile_do(Compiler* cs, Token* token) {
  enter_scope(cs);

  CompileResult cr;
  for (uint64_t i = 1; i < token->value.list.length; i++) {
    cr = compile(cs, token->value.list.data[i]);
  }

  leave_scope(cs);
  return cr;
}

CompileResult compile_binop(Compiler* cs, Token* token) {
  // Destruct
  Token* op = token->value.list.data[0];
  Token* lhs = token->value.list.data[1];
  Token* rhs = token->value.list.data[2];

  // Compile operands
  uint64_t save = cs->stack;
  CompileResult lhs_res = compile(cs, lhs);
  CompileResult rhs_res = compile(cs, rhs);
  cs->stack = save;  // Discard temp vars. They'll already be in cs->code

  // Check types
  if (lhs_res.type != rhs_res.type) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_DIFF_TYPES,
                           });
  }

  // Code gen
  char buffer[100] = {0};
  token_to_string(op, buffer);
  uint64_t dst = tmp(cs);  // Consume the next register
  array_append_fmt(cs->code, "binop %s %lld %lld %lld", buffer, lhs_res.reg,
                   rhs_res.reg, dst);

  return (CompileResult){
      .reg = dst,
      .type = lhs_res.type,
  };
}

CompileResult compile_list(Compiler* cs, Token* token) {
  uint64_t length = token->value.list.length;

  // Example: (+ 1 2)
  if (token_is_op(token->value.list.data[0]->type) && length == 3) {
    return compile_binop(cs, token);
  }

  // Example: (var a 1)
  if (token->value.list.data[0]->type == TOKEN_VAR && length == 3) {
    return compile_var(cs, token);
  }

  // Example: (do ...)
  if (token->value.list.data[0]->type == TOKEN_DO && length > 1) {
    return compile_do(cs, token);
  }

  return (CompileResult){
      .reg = -1,
      .type = TYPE_VOID,
  };
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

CompileResult compile(Compiler* cs, Token* token) {
  if (token == NULL) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_EMPTY_PROGRAM,
                           });
  }

  switch (token->type) {
    case TOKEN_LIST: {
      return compile_list(cs, token);
      break;
    }
    case TOKEN_NUM: {
      uint64_t dst = tmp(cs);
      array_append_fmt(cs->code, "const %lld %lld", token->value.num, dst);
      return (CompileResult){
          .reg = dst,
          .type = TYPE_INT,
      };
      break;
    }

    default: {
      return (CompileResult){
          .reg = -1,
          .type = TYPE_VOID,
      };
      break;
    }
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

uint64_t tmp(Compiler* cs) { return cs->stack++; }
