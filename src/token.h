#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

typedef enum {
  TOKEN_NUM,
  TOKEN_CALL,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_PRINT,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_DO,
  TOKEN_DEF,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_THEN,
  TOKEN_ELSE,
  TOKEN_LOOP,
  TOKEN_BREAK,
  TOKEN_RETURN,
  TOKEN_TERNARY,
  TOKEN_VAR,
  TOKEN_SET,
  TOKEN_IF,
  TOKEN_GT,
  TOKEN_LE,
  TOKEN_EQ,
  TOKEN_NE,
  TOKEN_GE,
  TOKEN_LT,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_ADD,
  TOKEN_MINUS,
  TOKEN_MULT,
  TOKEN_DIV,
  TOKEN_MODULO,
  TOKEN_LIST,
  TOKEN_ILLEGAL,
} TokenType;

typedef struct Token {
  TokenType type;
  union {
    int64_t num;
    uint8_t identifier;
    uint8_t string;
    struct Token* list;
  } value;
} Token;

#endif  // TOKEN_H
