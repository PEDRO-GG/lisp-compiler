#include "../src/token.c"

#include "test.h"

void test_parse(void) {
  typedef struct {
    const char* input;
    const char* expected_str;
    Token expected_token;
  } ParseTest;

  TokenError err;
  Token* list1 = token_list_make(&err);
  TKN_PANIC(err);
  err = token_list_append(list1, &(Token){
                                     .type = TOKEN_NUM,
                                     .value.num = 123,
                                 });
  TKN_PANIC(err);

  ParseTest tests[] = {
      {
          .input = "  (  123  )  ",
          .expected_token = *list1,
          .expected_str = "(123)",
      },
      {
          .input = " 123  ",
          .expected_token =
              {
                  .type = TOKEN_NUM,
                  .value.num = 123,
              },
          .expected_str = "123",
      },
  };

  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    TokenError err;
    uint64_t idx = 0;
    ParseTest tst = tests[i];
    Token* tkn = parse(tst.input, &idx, &err);
    char buffer[100] = {0};
    token_to_string(tkn, buffer);

    TEST_EQ(err, TOKEN_ERROR_NIL);
    TEST_EQ(tkncmp(tkn, &tst.expected_token), true);
    TEST_STRCMP(buffer, tst.expected_str);
  }
}

int main(void) {
  ADD_TEST(test_parse);
  RUN_TESTS();
  return 0;
}
