#include "token.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char* keyword;
  size_t length;
} Keyword;

// Excludes list, identifier, string, illegal, count
static const Keyword keywords[] = {
    [TOKEN_CALL] = {"call", sizeof("call")},
    [TOKEN_PRINT] = {"print", sizeof("print")},
    [TOKEN_DO] = {"do", sizeof("do")},
    [TOKEN_DEF] = {"def", sizeof("def")},
    [TOKEN_TRUE] = {"true", sizeof("true")},
    [TOKEN_FALSE] = {"false", sizeof("false")},
    [TOKEN_THEN] = {"then", sizeof("then")},
    [TOKEN_ELSE] = {"else", sizeof("else")},
    [TOKEN_LOOP] = {"loop", sizeof("loop")},
    [TOKEN_BREAK] = {"break", sizeof("break")},
    [TOKEN_RETURN] = {"return", sizeof("return")},
    [TOKEN_TERNARY] = {"?", sizeof("?")},
    [TOKEN_VAR] = {"var", sizeof("var")},
    [TOKEN_SET] = {"set", sizeof("set")},
    [TOKEN_IF] = {"if", sizeof("if")},
    [TOKEN_GT] = {"gt", sizeof("gt")},
    [TOKEN_LE] = {"le", sizeof("le")},
    [TOKEN_EQ] = {"eq", sizeof("eq")},
    [TOKEN_NE] = {"ne", sizeof("ne")},
    [TOKEN_GE] = {"ge", sizeof("ge")},
    [TOKEN_LT] = {"lt", sizeof("lt")},
    [TOKEN_AND] = {"and", sizeof("and")},
    [TOKEN_OR] = {"or", sizeof("or")},
    [TOKEN_ADD] = {"+", sizeof("+")},
    [TOKEN_MINUS] = {"-", sizeof("-")},
    [TOKEN_MULT] = {"*", sizeof("*")},
    [TOKEN_DIV] = {"/", sizeof("/")},
    [TOKEN_MODULO] = {"%", sizeof("%")},
};

static const size_t keywords_length = sizeof(keywords) / sizeof(keywords[0]);

void token_to_string(const Token* t, char* buffer) {
  assert(t != NULL);
  assert(buffer != NULL);

  switch (t->type) {
    case TOKEN_NUM: {
      char num[100];
      sprintf(num, "%lld", t->value.num);
      strcat(buffer, num);
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
    case TOKEN_IDENTIFIER: {
      strncat(buffer, (char*)t->value.identifier.start,
              t->value.identifier.length);
      break;
    }
    case TOKEN_STRING: {
      strncat(buffer, (char*)t->value.string.start, t->value.string.length);
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
    case TOKEN_COUNT: {
      strcat(buffer, "count");
      break;
    }
    default: {
      if (t->type < keywords_length) {
        strcat(buffer, keywords[t->type].keyword);
      }
      break;
    }
  }
}

Token* token_list_make(Array* errs) {
  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    array_append(errs, &(Error){
                           .type = ERROR_MALLOC,
                       });
    return NULL;
  }

  uint64_t capacity = 10;
  Token** data = malloc(sizeof(Token*) * capacity);
  if (data == NULL) {
    array_append(errs, &(Error){
                           .type = ERROR_MALLOC,
                       });
    return NULL;
  }

  tkn->type = TOKEN_LIST;
  tkn->value.list = (Tokens){
      .data = data,
      .capacity = capacity,
      .length = 0,
  };

  return tkn;
}

void token_list_append(Token* list, Token* token, Array* errs) {
  assert(list != NULL);
  assert(list->type == TOKEN_LIST);

  // Request more memory if capacity is exceeded
  if (LENGTH(list) >= CAPACITY(list)) {
    uint64_t increase = 10;
    struct Token** tmp = realloc(DATA(list), sizeof(struct Token) * increase);
    if (tmp == NULL) {
      array_append(errs, &(Error){
                             .type = ERROR_REALLOC,
                         });
    }
    DATA(list) = tmp;
    CAPACITY(list) = CAPACITY(list) + increase;
  }

  // Append the token
  list->value.list.data[LENGTH(list)++] = token;
}

Token* token_list_init(Array* errs, int total, ...) {
  Token* list = token_list_make(errs);
  if (list == NULL) {
    return NULL;
  }

  va_list args;
  va_start(args, total);

  for (int i = 0; i < total; i++) {
    Token* tkn = va_arg(args, Token*);
    token_list_append(list, tkn, errs);
  }

  va_end(args);

  return list;
}

char skip_space(Parser* parser) {
  char c = get_curr_char(parser);
  while (isspace(c)) {
    c = read_char(parser);
  }
  return c;
}

Token* parse_num(Parser* parser, Array* errs) {
  int64_t num = 0;
  char c = get_curr_char(parser);
  while (isdigit(c)) {
    num *= 10;              // Left shift
    num += c - 48;          // Append digit
    c = read_char(parser);  // Move to next digit
  }

  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    array_append(errs, &(Error){
                           .type = ERROR_MALLOC,
                       });
    return NULL;
  }

  tkn->type = TOKEN_NUM;
  tkn->value.num = num;

  return tkn;
}

Token* parse_chars(Parser* parser, Array* errs) {
  uint64_t length = 0;
  const char* start = get_curr_ptr(parser);
  char c = get_curr_char(parser);
  while (isalpha(c) || is_op(c) || c == '_') {
    c = read_char(parser);
    length++;
  }

  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    array_append(errs, &(Error){
                           .type = ERROR_MALLOC,
                       });
    return NULL;
  }

  for (size_t i = 0; i < keywords_length; i++) {
    Keyword keyword = keywords[i];

    // Null terminator not considered
    if (keyword.length - 1 == length &&
        strncmp(keyword.keyword, start, length) == 0) {
      tkn->type = (TokenType)i;
      return tkn;
    }
  }

  tkn->type = TOKEN_IDENTIFIER;
  tkn->value.identifier = (FatStr){
      .start = (const uint8_t*)start,
      .length = length,
  };

  return tkn;
}

Token* parse_string(Parser* parser, Array* errs) {
  const char* start = get_curr_ptr(parser);
  uint64_t length = 1;
  char c = read_char(parser);
  while (c != '\"' && c != '\0') {
    c = read_char(parser);
    length++;
  }
  c = read_char(parser);
  length++;

  Token* tkn = malloc(sizeof(Token));
  if (tkn == NULL) {
    array_append(errs, &(Error){
                           .type = ERROR_MALLOC,
                       });
    return NULL;
  }

  tkn->type = TOKEN_STRING;
  tkn->value.string = (FatStr){
      .start = (const uint8_t*)start,
      .length = length,
  };

  return tkn;
}

Token* parse_value(Parser* parser, Array* errs) {
  char c = get_curr_char(parser);
  if (isdigit(c)) {
    return parse_num(parser, errs);
  } else if (c == '\"') {
    return parse_string(parser, errs);
  } else if (c == '-' && peek_char(parser) != ' ') {
    int operand = -1;
    c = read_char(parser);
    while (c == '-') {
      operand *= -1;
      c = read_char(parser);
    }
    Token* num = parse_num(parser, errs);
    num->value.num *= operand;
    return num;
  }
  return parse_chars(parser, errs);
}

Token* parse(Parser* parser) {
  assert(parser != NULL);
  assert(parser->errs != NULL);

  char c = skip_space(parser);
  if (c == '(') {
    c = read_char(parser);

    Token* parent_list = token_list_make(parser->errs);
    while (true) {
      c = skip_space(parser);

      if (c == '\0') {
        array_append(parser->errs, &(Error){
                                       .type = ERROR_UNBALANCED_PARENS,
                                   });
        return NULL;
      }

      if (c == ')') {
        c = read_char(parser);
        break;
      }

      Token* child = parse(parser);
      token_list_append(parent_list, child, parser->errs);
    }
    return parent_list;
  } else if (c == ')') {
    array_append(parser->errs, &(Error){
                                   .type = ERROR_UNBALANCED_PARENS,
                               });
    return NULL;
  } else if (c == '\0') {
    array_append(parser->errs, &(Error){
                                   .type = ERROR_EMPTY_PROGRAM,
                               });
    return NULL;
  }

  return parse_value(parser, parser->errs);
}

bool tkncmp(const Token* t1, const Token* t2) {
  if (t1 == NULL || t2 == NULL || t1->type != t2->type) {
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

bool token_is_op(TokenType t) {
  return t == TOKEN_ADD || t == TOKEN_MINUS || t == TOKEN_MULT ||
         t == TOKEN_DIV || t == TOKEN_EQ || t == TOKEN_NE || t == TOKEN_GE ||
         t == TOKEN_GT || t == TOKEN_LE || t == TOKEN_LT || t == TOKEN_AND ||
         t == TOKEN_OR;
}

void print_token(const Token* t) {
  char buffer[200] = {0};
  token_to_string(t, buffer);
  printf("%s", buffer);
}

Parser new_parser(const char* input) {
  return (Parser){
      .input = input,
      .col = 1,
      .row = 1,
      .idx = 0,
      .errs = array_new(10, sizeof(Error)),
  };
}
char get_curr_char(Parser* parser) { return parser->input[parser->idx]; }

char peek_char(Parser* parser) {
  char current = parser->input[parser->idx];
  if (current == '\0') return current;
  char next = parser->input[parser->idx + 1];
  return next;
}

char read_char(Parser* parser) {
  char current = parser->input[parser->idx];
  if (current == '\0') return current;

  char next = parser->input[++parser->idx];
  if (next == '\n') {
    parser->col = 0;
    parser->row++;
  } else {
    parser->col++;
  }

  return next;
}

const char* get_curr_ptr(Parser* parser) { return parser->input + parser->idx; }
