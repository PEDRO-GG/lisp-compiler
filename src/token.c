#include "token.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void to_string(Token* t, char* buffer) {
  assert(t != NULL);
  assert(buffer != NULL);

  switch (t->type) {
    case TOKEN_NUM: {
      char num[100];
      sprintf(num, "%lld", t->value.num);
      strcat(buffer, num);
      break;
    }
    case TOKEN_CALL: {
      strcat(buffer, "call");
      break;
    }
    case TOKEN_LPAREN: {
      strcat(buffer, "(");
      break;
    }
    case TOKEN_RPAREN: {
      strcat(buffer, ")");
      break;
    }
    case TOKEN_PRINT: {
      strcat(buffer, "print");
      break;
    }
    case TOKEN_IDENTIFIER: {
      strcat(buffer, (char*)t->value.identifier);
      break;
    }
    case TOKEN_STRING: {
      strcat(buffer, (char*)t->value.string);
      break;
    }
    case TOKEN_DO: {
      strcat(buffer, "do");
      break;
    }
    case TOKEN_DEF: {
      strcat(buffer, "def");
      break;
    }
    case TOKEN_TRUE: {
      strcat(buffer, "true");
      break;
    }
    case TOKEN_FALSE: {
      strcat(buffer, "false");
      break;
    }
    case TOKEN_THEN: {
      strcat(buffer, "then");
      break;
    }
    case TOKEN_ELSE: {
      strcat(buffer, "else");
      break;
    }
    case TOKEN_LOOP: {
      strcat(buffer, "loop");
      break;
    }
    case TOKEN_BREAK: {
      strcat(buffer, "break");
      break;
    }
    case TOKEN_RETURN: {
      strcat(buffer, "return");
      break;
    }
    case TOKEN_TERNARY: {
      strcat(buffer, "?");
      break;
    }
    case TOKEN_VAR: {
      strcat(buffer, "var");
      break;
    }
    case TOKEN_SET: {
      strcat(buffer, "set");
      break;
    }
    case TOKEN_IF: {
      strcat(buffer, "if");
      break;
    }
    case TOKEN_GT: {
      strcat(buffer, "gt");
      break;
    }
    case TOKEN_LE: {
      strcat(buffer, "le");
      break;
    }
    case TOKEN_EQ: {
      strcat(buffer, "eq");
      break;
    }
    case TOKEN_NE: {
      strcat(buffer, "ne");
      break;
    }
    case TOKEN_GE: {
      strcat(buffer, "ge");
      break;
    }
    case TOKEN_LT: {
      strcat(buffer, "lt");
      break;
    }
    case TOKEN_AND: {
      strcat(buffer, "and");
      break;
    }
    case TOKEN_OR: {
      strcat(buffer, "or");
      break;
    }
    case TOKEN_ADD: {
      strcat(buffer, "+");
      break;
    }
    case TOKEN_MINUS: {
      strcat(buffer, "-");
      break;
    }
    case TOKEN_MULT: {
      strcat(buffer, "*");
      break;
    }
    case TOKEN_DIV: {
      strcat(buffer, "/");
      break;
    }
    case TOKEN_MODULO: {
      strcat(buffer, "%");
      break;
    }
    case TOKEN_LIST: {
      for (uint64_t i = 0; i < LENGTH(t); i++) {
        to_string(DATA(t)[i], buffer);
        strcat(buffer, " ");
      }
      break;
    }
    case TOKEN_ILLEGAL: {
      strcat(buffer, "illegal");
      break;
    }
    default:
      break;
  }
}

Token* token_list_make(TokenError* err) {
  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    *err = TOKEN_ERROR_MALLOC;
    return NULL;
  }

  uint64_t capacity = 10;
  Token** data = malloc(sizeof(Token*) * capacity);
  if (data == NULL) {
    *err = TOKEN_ERROR_MALLOC;
    return NULL;
  }

  tkn->type = TOKEN_LIST;
  tkn->value.list = (Tokens){
      .data = data,
      .capacity = capacity,
      .length = 0,
  };
  *err = TOKEN_ERROR_NIL;
  return tkn;
}

TokenError token_list_append(Token* list, Token* token) {
  assert(list != NULL);
  assert(list->type == TOKEN_LIST);

  // Request more memory if capacity is exceeded
  if (LENGTH(list) >= CAPACITY(list)) {
    uint64_t increase = 10;
    struct Token** tmp = realloc(DATA(list), sizeof(struct Token) * increase);
    if (tmp == NULL) {
      return TOKEN_ERROR_REALLOC;
    }
    DATA(list) = tmp;
    CAPACITY(list) = CAPACITY(list) + increase;
  }

  // Append the token
  list->value.list.data[LENGTH(list)++] = token;
  return TOKEN_ERROR_NIL;
}
