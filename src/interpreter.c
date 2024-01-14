#include "interpreter.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define DEBUG 0  // Enable or disable debug prints

// Assumes the `TOKEN_LIST` variant is active and it has length of 3
// Example: (+ 1 2)
EvaluateError evaluate_operation(Token* token, Env* env, Result* result) {
  if (token->value.list.data[0] == NULL) {
    return EVALUATE_ERROR_EXPECTED_OP;
  }

  if (token->value.list.data[1] == NULL || token->value.list.data[2] == NULL) {
    return EVALUATE_ERROR_EXPECTED_OPERAND;
  }

  EvaluateError err;
  Result left;
  Result right;

  err = evaluate(token->value.list.data[1], env, &left);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  err = evaluate(token->value.list.data[2], env, &right);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  if (left.type != right.type) {
    return EVALUATE_ERROR_UNMATCHED_TYPES;
  }

  switch (token->value.list.data[0]->type) {
    case TOKEN_ADD: {
      result->type = RESULT_NUM;
      result->value.num = left.value.num + right.value.num;
      break;
    }
    case TOKEN_MINUS: {
      result->type = RESULT_NUM;
      result->value.num = left.value.num - right.value.num;
      break;
    }
    case TOKEN_MULT: {
      result->type = RESULT_NUM;
      result->value.num = left.value.num * right.value.num;
      break;
    }
    case TOKEN_DIV: {
      result->type = RESULT_NUM;
      result->value.num = left.value.num / right.value.num;
      break;
    }
    case TOKEN_EQ: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.boolean == right.value.boolean;
      break;
    }
    case TOKEN_NE: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.boolean != right.value.boolean;
      break;
    }
    case TOKEN_GE: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.num >= right.value.num;
      break;
    }
    case TOKEN_GT: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.num > right.value.num;
      break;
    }
    case TOKEN_LE: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.num <= right.value.num;
      break;
    }
    case TOKEN_LT: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.num < right.value.num;
      break;
    }
    case TOKEN_AND: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.boolean && right.value.boolean;
      break;
    }
    case TOKEN_OR: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.boolean || right.value.boolean;
      break;
    }
    default:
      return EVALUATE_ERROR_EXPECTED_OP;
  }

  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active
EvaluateError evaluate_do(Token* token, Env* env, Result* result) {
  EvaluateError err;
  Env* new_env = env_make(&err, env);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  for (uint64_t i = 1; i < token->value.list.length; i++) {
    err = evaluate(token->value.list.data[i], new_env, result);
    if (err != EVALUATE_ERROR_NIL) {
      return err;
    }
  }

  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active and length is 3 or 4
EvaluateError evaluate_if(Token* token, Env* env, Result* result) {
  EvaluateError err;
  Env* new_env;
  Result condition_result;

  new_env = env_make(&err, env);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  err = evaluate(token->value.list.data[1], new_env, &condition_result);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  if (condition_result.type == RESULT_BOOL) {
    if (condition_result.value.boolean == true) {
      return evaluate(token->value.list.data[2], new_env, result);
    }
    if (token->value.list.length == 4) {
      return evaluate(token->value.list.data[3], new_env, result);
    } else {  // No else
      result->type = RESULT_NOOP;
    }
  } else {
    return EVALUATE_ERROR_EXPECTED_BOOL;
  }

  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active and it has length of 3
// Example: (var a 1)
EvaluateError evaluate_var(Token* token, Env* env, Result* result) {
  if (env == NULL) {
    return EVALUATE_ERROR_NO_SCOPE;
  }

  Token* var_name = token->value.list.data[1];
  if (var_name->type != TOKEN_IDENTIFIER) {
    return EVALUATE_ERROR_EXPECTED_IDENT;
  }

  if (env_contains_var(env, &var_name->value.identifier)) {
    return EVALUATE_ERROR_DUPLICATE_IDENT;
  }

  EvaluateError err = evaluate(token->value.list.data[2], env, result);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  err = env_append(env, (Symbol){
                            .type = SYMBOL_VAR,
                            .value.var =
                                (Var){
                                    .name = var_name->value.identifier,
                                    .result = *result,
                                },
                        });
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active and it has length of 3
// Example: (set a 1)
EvaluateError evaluate_set(Token* token, Env* env, Result* result) {
  if (token->value.list.data[1]->type != TOKEN_IDENTIFIER) {
    return EVALUATE_ERROR_EXPECTED_IDENT;
  }

  if (env == NULL) {
    return EVALUATE_ERROR_NO_SCOPE;
  }

  Var* to_modify =
      env_find_var(env, &token->value.list.data[1]->value.identifier);
  if (to_modify == NULL) {
    return EVALUATE_ERROR_VAR_NOT_FOUND;
  }

  EvaluateError err = evaluate(token->value.list.data[2], env, result);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  to_modify->result = *result;

  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active and it has length of 3
// Example: (loop <condition> <body>)
EvaluateError evaluate_loop(Token* token, Env* env, Result* result) {
  EvaluateError err;
  Env* new_env;
  Result condition_result;

  new_env = env_make(&err, env);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  while (true) {
    err = evaluate(token->value.list.data[1], new_env, &condition_result);
    if (err != EVALUATE_ERROR_NIL) {
      return err;
    }

    if (condition_result.type == RESULT_BOOL) {
      if (condition_result.value.boolean == true) {
        err = evaluate(token->value.list.data[2], new_env, result);
        if (err == EVALUATE_ERROR_BREAK) {
          return EVALUATE_ERROR_NIL;
        }
        if (err != EVALUATE_ERROR_NIL) {
          return err;
        }
      } else {
        return EVALUATE_ERROR_NIL;
      }
    }
  }
  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active and it has length of 4
// Example: (def foo (arg1, arg2) body)
EvaluateError evaluate_func(Token* token, Env* env, Result* result) {
  assert(env != NULL);
  assert(result != NULL);

  Token* args = token->value.list.data[2];
  if (args->type != TOKEN_LIST) {
    return EVALUATE_ERROR_EXPECTED_LIST;
  }

  for (uint64_t i = 0; i < args->value.list.length; i++) {
    Token* arg = args->value.list.data[i];
    if (arg->type != TOKEN_IDENTIFIER) {
      return EVALUATE_ERROR_EXPECTED_IDENT;
    }
  }

  EvaluateError err = env_append(env, (Symbol){
                                          .type = SYMBOL_FUNC,
                                          .value.func = token,
                                      });
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  result->type = RESULT_NOOP;

  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active and it has length >= 2
// Example: (call foo args1 arg2)
// Example: (call foo)
EvaluateError evaluate_call(Token* token, Env* env, Result* result) {
  assert(env != NULL);
  assert(result != NULL);

  // Find the function
  Token* to_call =
      env_find_func(env, &token->value.list.data[1]->value.identifier);
  if (to_call == NULL) {
    return EVALUATE_ERROR_FUNC_NOT_FOUND;
  }

  // Initialize new scope
  EvaluateError err;
  Env* new_env = env_make(&err, env);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  // Evaluate each argument and store its result
  Token* params = to_call->value.list.data[2];
  uint64_t params_length = params->value.list.length;
  for (uint64_t i = 0; i < params_length; i++) {
    Result arg_result;
    Token* arg_to_eval = token->value.list.data[i + 2];
    err = evaluate(arg_to_eval, env, &arg_result);
    if (err != EVALUATE_ERROR_NIL) {
      return err;
    }

    err = env_append(
        new_env,
        (Symbol){
            .type = SYMBOL_VAR,
            .value.var =
                (Var){
                    .name = params->value.list.data[i]->value.identifier,
                    .result = arg_result,
                },
        });
    if (err != EVALUATE_ERROR_NIL) {
      return err;
    }
  }

  // Evaluate the body
  err = evaluate(to_call->value.list.data[3], new_env, result);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  return EVALUATE_ERROR_NIL;
}

EvaluateError evaluate_list(Token* token, Env* env, Result* result) {
  uint64_t length = token->value.list.length;

  if (length == 0) {
    return EVALUATE_ERROR_EMPTY_LIST;
  }

  // Example: (+ 1 2)
  if (token_is_op(token->value.list.data[0]->type) && length == 3) {
    return evaluate_operation(token, env, result);
  }

  // Example: (var a 1)
  if (token->value.list.data[0]->type == TOKEN_VAR && length == 3) {
    return evaluate_var(token, env, result);
  }

  // Example: (set a 1)
  if (token->value.list.data[0]->type == TOKEN_SET && length == 3) {
    return evaluate_set(token, env, result);
  }

  // Example: (do ...)
  if (token->value.list.data[0]->type == TOKEN_DO && length > 1) {
    return evaluate_do(token, env, result);
  }

  // Example: (if true \"YES\" \"NO\")
  // Example: (if true \"YES\")
  if (token->value.list.data[0]->type == TOKEN_IF &&
      (length == 3 || length == 4)) {
    return evaluate_if(token, env, result);
  }

  // Example: (loop <condition> <body> )
  if (token->value.list.data[0]->type == TOKEN_LOOP && length == 3) {
    return evaluate_loop(token, env, result);
  }

  // Example: (break)
  if (token->value.list.data[0]->type == TOKEN_BREAK && length == 1) {
    return EVALUATE_ERROR_BREAK;
  }

  // Example: (def foo (arg1, arg2) body)
  if (token->value.list.data[0]->type == TOKEN_DEF && length == 4) {
    return evaluate_func(token, env, result);
  }

  // Example: (call foo args1 arg2)
  if (token->value.list.data[0]->type == TOKEN_CALL && length >= 2) {
    return evaluate_call(token, env, result);
  }

  return EVALUATE_ERROR_NIL;
}

EvaluateError evaluate(Token* token, Env* env, Result* result) {
  assert(result != NULL);

#if DEBUG
  printf("\nevaluate(");
  print_token(token);
  printf(", ");
  print_env(env);
  printf(")");
  printf("\n");
#endif

  if (token == NULL) {
    return EVALUATE_ERROR_NULL_TOKEN;
  }

  switch (token->type) {
    case TOKEN_NUM: {
      result->type = RESULT_NUM;
      result->value.num = token->value.num;
      break;
    }
    case TOKEN_STRING: {
      result->type = RESULT_STRING;
      result->value.string = token->value.string;
      break;
    }
    case TOKEN_TRUE: {
      result->type = RESULT_BOOL;
      result->value.boolean = true;
      break;
    }
    case TOKEN_FALSE: {
      result->type = RESULT_BOOL;
      result->value.boolean = false;
      break;
    }
    case TOKEN_LIST: {
      return evaluate_list(token, env, result);
    }
    case TOKEN_IDENTIFIER: {
      Var* found = env_find_var(env, &token->value.identifier);
      if (found == NULL) {
        return EVALUATE_ERROR_VAR_NOT_FOUND;
      }
      (*result) = found->result;
      break;
    }
    default: {
      return EVALUATE_ERROR_ILLEGAL_TOKEN;
    }
  }

  return EVALUATE_ERROR_NIL;
}

bool rescmp(const Result* r1, const Result* r2) {
  if (r1 == NULL || r2 == NULL || r1->type != r2->type) {
    return false;
  }

  switch (r1->type) {
    case RESULT_NUM: {
      return r1->value.num == r2->value.num;
    }
    case RESULT_BOOL: {
      return r1->value.boolean == r2->value.boolean;
    }
    case RESULT_STRING: {
      return strncmp((char*)r1->value.string.start,
                     (char*)r2->value.string.start,
                     (size_t)r1->value.string.length) == 0;
    }
    case RESULT_NOOP: {
      return true;
    }
    default:
      return false;
  }
}

bool varcmp(const Symbol* s1, const Symbol* s2) {
  if (s1->type != s2->type) return false;

  if (s1->type == SYMBOL_VAR) {
    if (!fatstr_cmp(&s1->value.var.name, &s2->value.var.name)) return false;
    if (!rescmp(&s1->value.var.result, &s2->value.var.result)) return false;
  } else if (s1->type == SYMBOL_FUNC) {
    if (s1->value.func != s2->value.func) return false;
  }

  return true;
}

Env* env_make(EvaluateError* err, Env* next) {
  Env* env = malloc(sizeof(Env));
  if (env == NULL) {
    *err = EVALUATE_ERROR_MALLOC;
    return NULL;
  }

  env->capacity = 10;
  env->length = 0;
  env->next = next;
  env->data = malloc(sizeof(Symbol) * env->capacity);
  if (env->data == NULL) {
    *err = EVALUATE_ERROR_MALLOC;
    return NULL;
  }

  *err = EVALUATE_ERROR_NIL;

  return env;
}

EvaluateError env_append(Env* env, Symbol sym) {
  assert(env != NULL);

  // Request more memory if capacity is exceeded
  if (env->length >= env->capacity) {
    Symbol* tmp = realloc(env->data, sizeof(Symbol) * 10);
    if (tmp == NULL) {
      return EVALUATE_ERROR_REALLOC;
    }
    env->data = tmp;
    env->capacity += 10;
  }

  // Append
  env->data[env->length++] = sym;
  return EVALUATE_ERROR_NIL;
}

bool env_contains_var(Env* env, FatStr* str) {
  if (env == NULL || str == NULL) return false;

  for (uint64_t i = 0; i < env->length; i++) {
    if (env->data[i].type == SYMBOL_VAR &&
        fatstr_cmp(&env->data[i].value.var.name, str)) {
      return true;
    }
  }

  return false;
}

Var* env_find_var(Env* env, FatStr* str) {
  if (env == NULL || str == NULL) return NULL;

  for (uint64_t i = 0; i < env->length; i++) {
    if (env->data[i].type == SYMBOL_VAR &&
        fatstr_cmp(&env->data[i].value.var.name, str)) {
      return &env->data[i].value.var;
    }
  }

  // Go up a level
  if (env->next != NULL) {
    return env_find_var(env->next, str);
  }

  return NULL;
}

Token* env_find_func(Env* env, FatStr* str) {
  if (env == NULL || str == NULL) return NULL;

  for (uint64_t i = 0; i < env->length; i++) {
    if (env->data[i].type == SYMBOL_FUNC &&
        fatstr_cmp(
            &env->data[i].value.func->value.list.data[1]->value.identifier,
            str)) {
      return env->data[i].value.func;
    }
  }

  // Go up a level
  if (env->next != NULL) {
    return env_find_func(env->next, str);
  }

  return NULL;
}

void env_to_string(Env* env, char* buffer) {
  if (env == NULL) {
    strcat(buffer, "NULL");
    return;
  }

  strcat(buffer, "({");
  for (uint64_t i = 0; i < env->length; i++) {
    Symbol curr = env->data[i];
    if (curr.type == SYMBOL_FUNC) {
      fatstr_to_str(&curr.value.func->value.list.data[1]->value.identifier,
                    buffer);
      strcat(buffer, ": ()=>{}");
    } else {
      strcat(buffer, "'");
      fatstr_to_str(&curr.value.var.name, buffer);
      strcat(buffer, "': ");
      result_to_string(&curr.value.var.result, buffer);
    }
    if (i != env->length - 1) {
      strcat(buffer, ", ");
    }
  }
  strcat(buffer, "}, ");
  env_to_string(env->next, buffer);
  strcat(buffer, ")\0");
}

void print_env(Env* env) {
  char buffer[1024] = {0};
  env_to_string(env, buffer);
  printf("%s", buffer);
}

bool result_to_string(const Result* r, char* buffer) {
  switch (r->type) {
    case RESULT_NUM: {
      char num[100];
      sprintf(num, "%lld", r->value.num);
      strcat(buffer, num);
      break;
    }
    case RESULT_BOOL: {
      if (r->value.boolean) {
        strcat(buffer, "true");
      } else {
        strcat(buffer, "false");
      }
      break;
    }
    case RESULT_STRING: {
      return fatstr_to_str(&r->value.string, buffer);
    }
    case RESULT_NOOP: {
      strcat(buffer, "NOOP");
      break;
    }
    default:
      return false;
  }

  return true;
}
