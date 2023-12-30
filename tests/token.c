#include "../src/token.c"

#include "test.h"

void run_parse_test(const char* input, const Token* expected_tkn,
                    const char* expected_str) {
  TokenError err;
  uint64_t idx = 0;
  Token* tkn = parse(input, &idx, &err);
  char buffer[500] = {0};  // TODO: Change to dynamic memory allocation
  token_to_string(tkn, buffer);

  TEST_EQ(err, TOKEN_ERROR_NIL);
  if (expected_tkn) {
    TEST_EQ(tkncmp(tkn, expected_tkn), true);
  }
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
  run_parse_test("  \"yes  \" ",
                 &(Token){
                     .type = TOKEN_STRING,
                     .value.string =
                         (FatStr){
                             .start = (const uint8_t*)"\"yes  \"",
                             .length = 7,
                         },
                 },
                 "\"yes  \"");
}

void test_parse_ternary(void) {
  TokenError err;
  Token* list1;
  Token* list2;

  list2 = token_list_make(&err);
  TKN_PANIC(err);

  APPEND_TOKEN_AND_CHECK(list2, ((Token){.type = TOKEN_LT}));
  APPEND_TOKEN_AND_CHECK(list2, ((Token){.type = TOKEN_NUM, .value.num = 1}));
  APPEND_TOKEN_AND_CHECK(list2, ((Token){.type = TOKEN_NUM, .value.num = 2}));

  list1 = token_list_make(&err);
  TKN_PANIC(err);

  APPEND_TOKEN_AND_CHECK(list1, ((Token){.type = TOKEN_TERNARY}));
  APPEND_TOKEN_AND_CHECK(list1, (*list2));
  APPEND_TOKEN_AND_CHECK(list1, ((Token){
                                    .type = TOKEN_STRING,
                                    .value.string =
                                        (FatStr){
                                            .start = (const uint8_t*)"\"yes\"",
                                            .length = 5,
                                        },
                                }));
  APPEND_TOKEN_AND_CHECK(list1, ((Token){
                                    .type = TOKEN_STRING,
                                    .value.string =
                                        (FatStr){
                                            .start = (const uint8_t*)"\"no\"",
                                            .length = 4,
                                        },
                                }));

  run_parse_test("(  ? (lt 1 2) \"yes\"   \"no\")", list1,
                 "(? (lt 1 2) \"yes\" \"no\")");
}

void test_parse_var(void) {
  TokenError err;
  run_parse_test("(var x y)",
                 token_list_init(&err, 3,
                                 &(Token){
                                     .type = TOKEN_VAR,
                                 },
                                 &(Token){
                                     .type = TOKEN_IDENTIFIER,
                                     .value.identifier =
                                         (FatStr){
                                             .start = (uint8_t*)"x",
                                             .length = 1,
                                         },
                                 },
                                 &(Token){
                                     .type = TOKEN_IDENTIFIER,
                                     .value.identifier =
                                         (FatStr){
                                             .start = (uint8_t*)"y",
                                             .length = 1,
                                         },
                                 }),
                 "(var x y)");
  TKN_PANIC(err);
}

void test_parse_do(void) {
  char* input =
      "(do"
      "  (var x 1)"
      "  (var y 2)"
      "  (do"
      "      (var x (+ y 3))"  // x = 5
      "      (set y (+ x 4))"  // y = 9
      "  )"
      "  (+ x y)"  // return 10
      ")";

  char* expected_str =
      "(do (var x 1) (var y 2) (do (var x (+ y 3)) (set y (+ x 4))) (+ x y))";

  TokenError err;
  Token* main = token_list_init(
      &err, 5,
      &(Token){
          .type = TOKEN_DO,
      },
      token_list_init(&err, 3,
                      &(Token){
                          .type = TOKEN_VAR,
                      },
                      &(Token){
                          .type = TOKEN_IDENTIFIER,
                          .value.identifier =
                              (FatStr){
                                  .start = (uint8_t*)"x",
                                  .length = 1,
                              },
                      },
                      &(Token){
                          .type = TOKEN_NUM,
                          .value.num = 1,
                      }),
      token_list_init(&err, 3,
                      &(Token){
                          .type = TOKEN_VAR,
                      },
                      &(Token){
                          .type = TOKEN_IDENTIFIER,
                          .value.identifier =
                              (FatStr){
                                  .start = (uint8_t*)"y",
                                  .length = 1,
                              },
                      },
                      &(Token){
                          .type = TOKEN_NUM,
                          .value.num = 2,
                      }),
      token_list_init(
          &err, 3,
          &(Token){
              .type = TOKEN_DO,
          },
          token_list_init(&err, 3,
                          &(Token){
                              .type = TOKEN_VAR,
                          },
                          &(Token){
                              .type = TOKEN_IDENTIFIER,
                              .value.identifier =
                                  (FatStr){
                                      .start = (uint8_t*)"x",
                                      .length = 1,
                                  },
                          },
                          token_list_init(&err, 3,
                                          &(Token){
                                              .type = TOKEN_ADD,
                                          },
                                          &(Token){
                                              .type = TOKEN_IDENTIFIER,
                                              .value.identifier =
                                                  (FatStr){
                                                      .start = (uint8_t*)"y",
                                                      .length = 1,
                                                  },
                                          },
                                          &(Token){
                                              .type = TOKEN_NUM,
                                              .value.num = 3,
                                          })),
          token_list_init(&err, 3,
                          &(Token){
                              .type = TOKEN_SET,
                          },
                          &(Token){
                              .type = TOKEN_IDENTIFIER,
                              .value.identifier =
                                  (FatStr){
                                      .start = (uint8_t*)"y",
                                      .length = 1,
                                  },
                          },
                          token_list_init(&err, 3,
                                          &(Token){
                                              .type = TOKEN_ADD,
                                          },
                                          &(Token){
                                              .type = TOKEN_IDENTIFIER,
                                              .value.identifier =
                                                  (FatStr){
                                                      .start = (uint8_t*)"x",
                                                      .length = 1,
                                                  },
                                          },
                                          &(Token){
                                              .type = TOKEN_NUM,
                                              .value.num = 4,
                                          }))),
      token_list_init(&err, 3,
                      &(Token){
                          .type = TOKEN_ADD,
                      },
                      &(Token){
                          .type = TOKEN_IDENTIFIER,
                          .value.identifier =
                              (FatStr){
                                  .start = (uint8_t*)"x",
                                  .length = 1,
                              },
                      },
                      &(Token){
                          .type = TOKEN_IDENTIFIER,
                          .value.identifier =
                              (FatStr){
                                  .start = (uint8_t*)"y",
                                  .length = 1,
                              },
                      }));
  TKN_PANIC(err);

  run_parse_test(input, main, expected_str);
}

void test_parse_func(void) {
  run_parse_test(
      "(do "
      "    (def fibonacci (n)"
      "        (if (le n 0)"
      "            (then 0)"
      "            (else (+ n (call fibonacci (- n 1))))))"
      "    (call fibonacci 10)"
      ")",
      NULL,
      "(do (def fibonacci (n) (if (le n 0) (then 0) (else (+ n (call fibonacci "
      "(- n "
      "1)))))) (call fibonacci 10))");
}

void test_parse_loop(void) {
  run_parse_test(
      "(do"
      "    (var a 0)"
      "    (var b 0)"
      "    (loop true (do"
      "        (set a (+ a 1))"
      "        (set b (+ b 1))"
      "        (if (gt (+ a b) 10) (break))"
      "    ))"
      "    (* a b)"
      ")",
      NULL,
      "(do (var a 0) (var b 0) (loop true (do (set a (+ a 1)) (set b (+ b 1)) "
      "(if (gt (+ a b) 10) (break)))) (* a b))");
}

int main(void) {
  ADD_TEST(test_parse_num);
  ADD_TEST(test_parse_list_with_one_element);
  ADD_TEST(test_parse_list_with_two_elements);
  ADD_TEST(test_parse_arithmetic);
  ADD_TEST(test_parse_string);
  ADD_TEST(test_parse_ternary);
  ADD_TEST(test_parse_var);
  ADD_TEST(test_parse_do);
  ADD_TEST(test_parse_func);
  ADD_TEST(test_parse_loop);
  RUN_TESTS();
  return 0;
}
