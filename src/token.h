#ifndef TOKEN_H
#define TOKEN_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "errors.h"
#include "fatstr.h"

typedef enum {
  /* Keywords */
  TOKEN_CALL = 0,
  TOKEN_PRINT,
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

  /* Special tokens */
  TOKEN_NUM,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_IDENTIFIER,
  TOKEN_STRING,
  TOKEN_ILLEGAL,
  TOKEN_LIST,
  TOKEN_COUNT,
} TokenType;

typedef struct Tokens {
  struct Token** data;
  uint64_t capacity;
  uint64_t length;
} Tokens;

typedef struct Token {
  TokenType type;
  union {
    int64_t num;
    FatStr identifier;
    FatStr string;
    Tokens list;
  } value;
} Token;

#define CAPACITY(x) ((x)->value.list.capacity)
#define LENGTH(x) ((x)->value.list.length)
#define DATA(x) ((x)->value.list.data)

Token* token_list_make(Errors* errs);
Token* token_list_init(Errors* errs, int total, ...);
void token_list_append(Token* list, Token* token, Errors* errs);

bool tkncmp(const Token* t1, const Token* t2);
void token_to_string(const Token* t, char* buffer);
void print_token(const Token* t);
bool is_op(char c);
bool token_is_op(TokenType t);
Token* parse(const char* input, uint64_t* idx, Errors* errs);

#endif  // TOKEN_H
