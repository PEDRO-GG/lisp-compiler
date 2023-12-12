#include "token.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void to_string(Token* t, char* buffer) {
  assert(t != NULL);
  assert(buffer != NULL);

  switch (t->type) {
    case TOKEN_NUM: {
      sprintf(buffer, "%lld", t->value.num);
      break;
    }
    case TOKEN_CALL: {
      strcpy(buffer, "call");
      break;
    }
    case TOKEN_LPAREN: {
      strcpy(buffer, "(");
      break;
    }
    case TOKEN_RPAREN: {
      strcpy(buffer, ")");
      break;
    }
    case TOKEN_PRINT: {
      strcpy(buffer, "print");
      break;
    }
    case TOKEN_IDENTIFIER: {
      strcpy(buffer, (char*)t->value.identifier);
      break;
    }
    case TOKEN_STRING: {
      strcpy(buffer, (char*)t->value.string);
      break;
    }
    case TOKEN_DO: {
      strcpy(buffer, "do");
      break;
    }
    case TOKEN_DEF: {
      strcpy(buffer, "def");
      break;
    }
    case TOKEN_TRUE: {
      strcpy(buffer, "true");
      break;
    }
    case TOKEN_FALSE: {
      strcpy(buffer, "false");
      break;
    }
    case TOKEN_THEN: {
      strcpy(buffer, "then");
      break;
    }
    case TOKEN_ELSE: {
      strcpy(buffer, "else");
      break;
    }
    case TOKEN_LOOP: {
      strcpy(buffer, "loop");
      break;
    }
    case TOKEN_BREAK: {
      strcpy(buffer, "break");
      break;
    }
    case TOKEN_RETURN: {
      strcpy(buffer, "return");
      break;
    }
    case TOKEN_TERNARY: {
      strcpy(buffer, "?");
      break;
    }
    case TOKEN_VAR: {
      strcpy(buffer, "var");
      break;
    }
    case TOKEN_SET: {
      strcpy(buffer, "set");
      break;
    }
    case TOKEN_IF: {
      strcpy(buffer, "if");
      break;
    }
    case TOKEN_GT: {
      strcpy(buffer, "gt");
      break;
    }
    case TOKEN_LE: {
      strcpy(buffer, "le");
      break;
    }
    case TOKEN_EQ: {
      strcpy(buffer, "eq");
      break;
    }
    case TOKEN_NE: {
      strcpy(buffer, "ne");
      break;
    }
    case TOKEN_GE: {
      strcpy(buffer, "ge");
      break;
    }
    case TOKEN_LT: {
      strcpy(buffer, "lt");
      break;
    }
    case TOKEN_AND: {
      strcpy(buffer, "and");
      break;
    }
    case TOKEN_OR: {
      strcpy(buffer, "or");
      break;
    }
    case TOKEN_ADD: {
      strcpy(buffer, "+");
      break;
    }
    case TOKEN_MINUS: {
      strcpy(buffer, "-");
      break;
    }
    case TOKEN_MULT: {
      strcpy(buffer, "*");
      break;
    }
    case TOKEN_DIV: {
      strcpy(buffer, "/");
      break;
    }
    case TOKEN_MODULO: {
      strcpy(buffer, "%");
      break;
    }
    case TOKEN_LIST: {
      strcpy(buffer, "list");
      break;
    }
    case TOKEN_ILLEGAL: {
      strcpy(buffer, "illegal");
      break;
    }

    default:
      break;
  }
}
