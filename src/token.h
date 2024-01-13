#ifndef TOKEN_H
#define TOKEN_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

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

typedef enum {
  TOKEN_ERROR_MALLOC,
  TOKEN_ERROR_REALLOC,
  TOKEN_ERROR_APPEND,
  TOKEN_ERROR_UNBALANCED_PARENS,
  TOKEN_ERROR_EXPECTED_LPAREN,
  TOKEN_ERROR_EMPTY_PROGRAM,
  TOKEN_ERROR_NIL,
} TokenError;

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

Token* token_list_make(TokenError* err);
Token* token_list_init(TokenError* err, int total, ...);
TokenError token_list_append(Token* list, Token* token);

bool tkncmp(const Token* t1, const Token* t2);
void token_to_string(const Token* t, char* buffer);
void print_token(const Token* t);
bool is_op(char c);
bool token_is_op(TokenType t);
Token* parse(const char* input, uint64_t* idx, TokenError* err);

#define TKN_PANIC(err)            \
  do {                            \
    if (err != TOKEN_ERROR_NIL) { \
      exit(EXIT_FAILURE);         \
    }                             \
  } while (0)

#define APPEND_TOKEN_AND_CHECK(list, token_expr)  \
  do {                                            \
    err = token_list_append(list, &(token_expr)); \
    TKN_PANIC(err);                               \
  } while (0)

#endif  // TOKEN_H
