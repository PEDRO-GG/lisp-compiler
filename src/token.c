#include "token.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void token_to_string(Token* t, char* buffer) {
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
      strncat(buffer, (char*)t->value.identifier.start,
              t->value.identifier.length);
      strcat(buffer, "\0");
      break;
    }
    case TOKEN_STRING: {
      strncat(buffer, (char*)t->value.string.start, t->value.string.length);
      strcat(buffer, "\0");
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
      strcat(buffer, "(");
      for (uint64_t i = 0; i < LENGTH(t); i++) {
        token_to_string(DATA(t)[i], buffer);
        if (i < LENGTH(t) - 1) {
          strcat(buffer, " ");
        }
      }
      strcat(buffer, ")");
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

void skip_space(const char* input, uint64_t* idx) {
  while (isspace(input[*idx])) {
    (*idx)++;
  }
}

Token* parse_num(const char* input, uint64_t* idx, TokenError* err) {
  int64_t num = 0;
  while (isdigit(input[*idx])) {
    num *= 10;                // Left shift
    num += input[*idx] - 48;  // Append digit
    (*idx)++;                 // Move to next digit
  }

  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    *err = TOKEN_ERROR_MALLOC;
    return NULL;
  }

  tkn->type = TOKEN_NUM;
  tkn->value.num = num;
  *err = TOKEN_ERROR_NIL;

  return tkn;
}

Token* parse_chars(const char* input, uint64_t* idx, TokenError* err) {
  uint64_t length = 0;
  const char* start = input + *idx;
  while (isalpha(input[*idx]) || is_op(input[*idx])) {
    (*idx)++;
    length++;
  }

  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    *err = TOKEN_ERROR_MALLOC;
    return NULL;
  }

  if (strncmp(start, "call", length) == 0) {
    tkn->type = TOKEN_CALL;
  } else if (strncmp(start, "print", length) == 0) {
    tkn->type = TOKEN_PRINT;
  } else if (strncmp(start, "do", length) == 0) {
    tkn->type = TOKEN_DO;
  } else if (strncmp(start, "def", length) == 0) {
    tkn->type = TOKEN_DEF;
  } else if (strncmp(start, "then", length) == 0) {
    tkn->type = TOKEN_THEN;
  } else if (strncmp(start, "else", length) == 0) {
    tkn->type = TOKEN_ELSE;
  } else if (strncmp(start, "loop", length) == 0) {
    tkn->type = TOKEN_LOOP;
  } else if (strncmp(start, "break", length) == 0) {
    tkn->type = TOKEN_BREAK;
  } else if (strncmp(start, "return", length) == 0) {
    tkn->type = TOKEN_RETURN;
  } else if (strncmp(start, "var", length) == 0) {
    tkn->type = TOKEN_VAR;
  } else if (strncmp(start, "set", length) == 0) {
    tkn->type = TOKEN_SET;
  } else if (strncmp(start, "if", length) == 0) {
    tkn->type = TOKEN_IF;
  } else if (strncmp(start, "true", length) == 0) {
    tkn->type = TOKEN_TRUE;
  } else if (strncmp(start, "false", length) == 0) {
    tkn->type = TOKEN_FALSE;
  } else if (strncmp(start, "?", length) == 0) {
    tkn->type = TOKEN_TERNARY;
  } else if (strncmp(start, "gt", length) == 0) {
    tkn->type = TOKEN_GT;
  } else if (strncmp(start, "le", length) == 0) {
    tkn->type = TOKEN_LE;
  } else if (strncmp(start, "lt", length) == 0) {
    tkn->type = TOKEN_LT;
  } else if (strncmp(start, "+", length) == 0) {
    tkn->type = TOKEN_ADD;
  } else if (strncmp(start, "-", length) == 0) {
    tkn->type = TOKEN_MINUS;
  } else if (strncmp(start, "*", length) == 0) {
    tkn->type = TOKEN_MULT;
  } else {
    tkn->type = TOKEN_IDENTIFIER;
    tkn->value.identifier = (FatStr){
        .start = (const uint8_t*)input,
        .length = length,
    };
  }

  return tkn;
}

Token* parse_string(const char* input, uint64_t* idx, TokenError* err) {
  const char* start = input + *idx;
  uint64_t length = 1;
  (*idx)++;
  while (input[*idx] != '\"' && input[*idx] != '\0' && !isspace(input[*idx])) {
    (*idx)++;
    length++;
  }
  (*idx)++;
  length++;

  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    *err = TOKEN_ERROR_MALLOC;
    return NULL;
  }

  tkn->type = TOKEN_STRING;
  tkn->value.string = (FatStr){
      .start = (const uint8_t*)start,
      .length = length,
  };

  return tkn;
}

Token* parse_value(const char* input, uint64_t* idx, TokenError* err) {
  if (isdigit(input[*idx])) {
    return parse_num(input, idx, err);
  } else if (input[*idx] == '\"') {
    return parse_string(input, idx, err);
  }
  return parse_chars(input, idx, err);
}

Token* parse(const char* input, uint64_t* idx, TokenError* err) {
  assert(input != NULL);
  assert(err != NULL);

  *err = TOKEN_ERROR_NIL;

  skip_space(input, idx);

  if (input[*idx] == '(') {
    (*idx)++;

    Token* parent_list = token_list_make(err);
    if (*err != TOKEN_ERROR_NIL) {
      return NULL;
    }

    while (true) {
      skip_space(input, idx);

      if (input[*idx] == '\0') {
        *err = TOKEN_ERROR_UNBALANCED_PARENS;
      }

      if (input[*idx] == ')') {
        (*idx)++;
        break;
      }

      Token* child = parse(input, idx, err);
      if (*err != TOKEN_ERROR_NIL) {
        return NULL;
      }

      token_list_append(parent_list, child);
    }
    return parent_list;
  } else if (input[*idx] == ')') {
    *err = TOKEN_ERROR_EXPECTED_LPAREN;
    return NULL;
  } else if (input[*idx] == '\0') {
    *err = TOKEN_ERROR_EMPTY_PROGRAM;
    return NULL;
  }

  return parse_value(input, idx, err);
}

bool tkncmp(const Token* t1, const Token* t2) {
  if (t1->type != t2->type) {
    return false;
  }

  switch (t1->type) {
    case TOKEN_NUM: {
      return t1->value.num == t2->value.num;
    }
    case TOKEN_IDENTIFIER: {
      return strncmp((char*)t1->value.identifier.start,
                     (char*)t2->value.identifier.start,
                     (size_t)t1->value.identifier.length) == 0;
    }
    case TOKEN_STRING: {
      return strncmp((char*)t1->value.string.start,
                     (char*)t2->value.string.start,
                     (size_t)t1->value.string.length) == 0;
    }
    case TOKEN_LIST: {
      for (uint64_t i = 0; i < LENGTH(t1); i++) {
        Token* child1 = DATA(t1)[i];
        Token* child2 = DATA(t2)[i];
        if (!tkncmp(child1, child2)) {
          return false;
        }
      }
      return true;
    }
    default:
      return true;
  }
}

bool is_op(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/' || c == '?';
}
