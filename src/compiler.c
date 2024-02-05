#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// void compile_do(Compiler* cs, Token* token) {
//   enter_scope(cs);

//   for (uint64_t i = 1; i < token->value.list.length; i++) {
//     compile(cs, token->value.list.data[i]);
//   }

//   leave_scope(cs);
// }

// void compile_list(Compiler* cs, Token* token) {
//   uint64_t length = token->value.list.length;

//   // Example: (var a 1)
//   if (token->value.list.data[0]->type == TOKEN_VAR && length == 3) {
//     return evaluate_var(token, env, result);
//   }

//   // Example: (do ...)
//   if (token->value.list.data[0]->type == TOKEN_DO && length > 1) {
//     compile_do(cs, token);
//   }
// }

Compiler new_compiler(Token* token) {
  Compiler compiler = (Compiler){
      .idents = array_new(10, sizeof(Identifier)),
      .scopes = array_new(10, sizeof(uint64_t)),
      .code = array_new(10, sizeof(char)),
      .errs = errors_init(),
      .token = token,
      .stack = 0,
  };

  return compiler;
}

void compile(Compiler* cs) {
  Token* token = cs->token;
  if (token == NULL) {
    errors_append_fatal(cs->errs, (Error){
                                      .type = ERROR_EMPTY_PROGRAM,
                                  });
  }

  switch (token->type) {
    // case TOKEN_LIST: {
    //   compile_list(cs);
    //   break;
    // }
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
    errors_append_fatal(cs->errs, (Error){
                                      .type = ERROR_MALLOC,
                                  });
  }
}

void leave_scope(Compiler* cs) {
  size_t* current_scope = (size_t*)array_pop(cs->scopes);
  array_truncate(cs->idents, *current_scope);
}
