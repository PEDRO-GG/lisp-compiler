#include "interpreter.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

// Assumes the `TOKEN_LIST` variant is active and it has length of 3
// Example: (+ 1 2)
EvaluateError evalute_operation(Token* token, Result* result) {
  if (token->value.list.data[0] == NULL) {
    return EVALUATE_ERROR_EXPECTED_OP;
  }

  if (token->value.list.data[1] == NULL || token->value.list.data[2] == NULL) {
    return EVALUATE_ERROR_EXPECTED_OPERAND;
  }

  EvaluateError err;
  Result left;
  Result right;

  err = evalute(token->value.list.data[1], &left);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  err = evalute(token->value.list.data[2], &right);
  if (err != EVALUATE_ERROR_NIL) {
    return err;
  }

  if (token->value.list.data[1]->type != token->value.list.data[2]->type) {
    return EVALUATE_ERROR_UNMATCHED_TYPES;
  }

  result->type = RESULT_NUM;
  switch (token->value.list.data[0]->type) {
    case TOKEN_ADD: {
      result->value.num = left.value.num + right.value.num;
      break;
    }
    case TOKEN_MINUS: {
      result->value.num = left.value.num - right.value.num;
      break;
    }
    case TOKEN_MULT: {
      result->value.num = left.value.num * right.value.num;
      break;
    }
    case TOKEN_DIV: {
      result->value.num = left.value.num / right.value.num;
      break;
    }
    default:
      return EVALUATE_ERROR_EXPECTED_OP;
  }

  return EVALUATE_ERROR_NIL;
}

EvaluateError evalute_list(Token* token, Result* result) {
  uint64_t length = token->value.list.length;

  if (length == 0) {
    return EVALUATE_ERROR_EMPTY_LIST;
  }

  if (length == 3 && token_is_op(token->value.list.data[0]->type)) {
    return evalute_operation(token, result);
  }

  return EVALUATE_ERROR_NIL;
}

EvaluateError evalute(Token* token, Result* result) {
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
      return evalute_list(token, result);
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
