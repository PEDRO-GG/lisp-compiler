#include "../src/token.c"

#include "test.h"

void test_parse(void) {
  typedef struct {
    const char* input;
    const char* expected_str;
    Token expected_token;
  } ParseTest;

  ParseTest tests[] = {
      {
          .input = "  (  123  )  ",
          .expected_token =
              {
                  .type = TOKEN_NUM,
                  .value.num = 123,
              },
          .expected_str = "  (  1234  )  ",
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

    TEST_ASSERT(err == TOKEN_ERROR_NIL);

    char buffer[100];
    token_to_string(tkn, buffer);
  }
}

int main(void) {
  ADD_TEST(test_parse);
  ADD_TEST(test_parse);
  ADD_TEST(test_parse);
  ADD_TEST(test_parse);
  ADD_TEST(test_parse);
  ADD_TEST(test_parse);
  RUN_TESTS();
  return 0;
}
