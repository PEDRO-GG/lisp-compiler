#include "../src/token.c"

#include "test.h"

void test_parse(void) {
  typedef struct Test {
    const char* input;
    const char* expected_str;
    Token* expected_token;
  } Test;
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
