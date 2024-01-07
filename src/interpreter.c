#include "interpreter.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

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
      result->value.boolean = left.value.boolean >= right.value.boolean;
      break;
    }
    case TOKEN_GT: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.boolean > right.value.boolean;
      break;
    }
    case TOKEN_LE: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.boolean <= right.value.boolean;
      break;
    }
    case TOKEN_LT: {
      result->type = RESULT_BOOL;
      result->value.boolean = left.value.boolean < right.value.boolean;
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
  Env* new_env = env_make(&err, env);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  err = evaluate(token->value.list.data[1], new_env, result);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  if (result->type == RESULT_BOOL) {
    if (result->value.boolean == true) {
      return evaluate(token->value.list.data[2], new_env, result);
    }
    if (token->value.list.length == 4) {
      return evaluate(token->value.list.data[3], new_env, result);
    }
  } else {
    return EVALUATE_ERROR_EXPECTED_BOOL;
  }

  return EVALUATE_ERROR_NIL;
}

// Assumes the `TOKEN_LIST` variant is active and it has length of 3
// Example: (var a 1)
EvaluateError evaluate_var(Token* token, Env* env, Result* result) {
  if (token->value.list.data[1]->type != TOKEN_IDENTIFIER) {
    return EVALUATE_ERROR_EXPECTED_IDENT;
  }

  if (env == NULL) {
    return EVALUATE_ERROR_NO_SCOPE;
  }

  if (env_contains(env, &token->value.list.data[1]->value.identifier)) {
    return EVALUATE_ERROR_DUPLICATE_IDENT;
  }

  EvaluateError err = evaluate(token->value.list.data[2], env, result);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  err = env_append(env, (Var){
                            .name = token->value.list.data[1]->value.identifier,
                            .result = *result,
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

  Var* to_modify = env_find(env, &token->value.list.data[1]->value.identifier);
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

  return EVALUATE_ERROR_NIL;
}

EvaluateError evaluate(Token* token, Env* env, Result* result) {
  assert(result != NULL);

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
      Var* found = env_find(env, &token->value.identifier);
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
    default:
      return false;
  }
}

bool varcmp(const Var* v1, const Var* v2) {
  if (!fatstr_cmp(&v1->name, &v2->name)) return false;
  if (!rescmp(&v1->result, &v2->result)) return false;
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
  env->data = malloc(sizeof(Var) * env->capacity);
  if (env->data == NULL) {
    *err = EVALUATE_ERROR_MALLOC;
    return NULL;
  }

  *err = EVALUATE_ERROR_NIL;

  return env;
}

EvaluateError env_append(Env* env, Var var) {
  assert(env != NULL);

  // Request more memory if capacity is exceeded
  if (env->length >= env->capacity) {
    Var* tmp = realloc(env->data, sizeof(Var) * 10);
    if (tmp == NULL) {
      return EVALUATE_ERROR_REALLOC;
    }
    env->data = tmp;
    env->capacity += 10;
  }

  // Append
  env->data[env->length++] = var;
  return EVALUATE_ERROR_NIL;
}

bool env_contains(Env* env, FatStr* str) {
  if (env == NULL || str == NULL) return false;

  for (uint64_t i = 0; i < env->length; i++) {
    if (fatstr_cmp(&env->data[i].name, str)) {
      return true;
    }
  }

  return false;
}

Var* env_find(Env* env, FatStr* str) {
  if (env == NULL || str == NULL) return NULL;

  for (uint64_t i = 0; i < env->length; i++) {
    if (fatstr_cmp(&env->data[i].name, str)) {
      return env->data + i;
    }
  }

  if (env->next != NULL) {
    return env_find(env->next, str);
  }

  return NULL;
}
