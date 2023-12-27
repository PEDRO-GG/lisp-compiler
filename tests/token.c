#include "../src/token.c"

#include "test.h"

void run_parse_test(const char* input, const Token* expected_tkn,
                    const char* expected_str) {
  TokenError err;
  uint64_t idx = 0;
  Token* tkn = parse(input, &idx, &err);
  char buffer[100] = {0};
  token_to_string(tkn, buffer);

  TEST_EQ(err, TOKEN_ERROR_NIL);
  TEST_EQ(tkncmp(tkn, expected_tkn), true);
  TEST_STRCMP(buffer, expected_str);
}

void test_parse_num(void) {
  run_parse_test(" 123  ",
                 &(Token){
                     .type = TOKEN_NUM,
                     .value.num = 123,
                 },
                 "123");
}

void test_parse_list_with_one_element(void) {
  TokenError err;
  Token* list;

  list = token_list_make(&err);
  TKN_PANIC(err);

  err = token_list_append(list, &(Token){
                                    .type = TOKEN_NUM,
                                    .value.num = 123,
                                });
  TKN_PANIC(err);

  run_parse_test("  (  123  )  ", list, "(123)");
}

void test_parse_list_with_two_elements(void) {
  TokenError err;
  Token* list;

  list = token_list_make(&err);
  TKN_PANIC(err);

  err = token_list_append(list, &(Token){
                                    .type = TOKEN_NUM,
                                    .value.num = 123,
                                });
  TKN_PANIC(err);

  err = token_list_append(list, &(Token){
                                    .type = TOKEN_NUM,
                                    .value.num = 4,
                                });
  TKN_PANIC(err);

  run_parse_test("  (123 4) ", list, "(123 4)");
}

void test_parse_arithmetic(void) {
  TokenError err;
  Token* list1;
  Token* list2;

  list1 = token_list_make(&err);
  TKN_PANIC(err);

  err = token_list_append(list1, &(Token){
                                     .type = TOKEN_ADD,
                                 });
  TKN_PANIC(err);

  err = token_list_append(list1, &(Token){
                                     .type = TOKEN_NUM,
                                     .value.num = 1,
                                 });
  TKN_PANIC(err);

  err = token_list_append(list1, &(Token){
                                     .type = TOKEN_NUM,
                                     .value.num = 2,
                                 });
  TKN_PANIC(err);

  list2 = token_list_make(&err);
  TKN_PANIC(err);

  err = token_list_append(list2, &(Token){
                                     .type = TOKEN_MINUS,
                                 });
  TKN_PANIC(err);

  err = token_list_append(list2, &(Token){
                                     .type = TOKEN_NUM,
                                     .value.num = 1,
                                 });
  TKN_PANIC(err);

  err = token_list_append(list2, &(Token){
                                     .type = TOKEN_NUM,
                                     .value.num = 2,
                                 });
  TKN_PANIC(err);

  run_parse_test(" (   + 1 2   ) ", list1, "(+ 1 2)");
  run_parse_test(" (  - 1 2   ) ", list2, "(- 1 2)");
}

void test_parse_string(void) {
  run_parse_test("  \"yes\" ",
                 &(Token){
                     .type = TOKEN_STRING,
                     .value.string =
                         (FatStr){
                             .start = (const uint8_t*)"\"yes\"",
                             .length = 5,
                         },
                 },
                 "\"yes\"");
}

int main(void) {
  ADD_TEST(test_parse_num);
  ADD_TEST(test_parse_list_with_one_element);
  ADD_TEST(test_parse_list_with_two_elements);
  ADD_TEST(test_parse_arithmetic);
  ADD_TEST(test_parse_string);
  RUN_TESTS();
  return 0;
}
