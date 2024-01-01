#include "../src/interpreter.c"

#include "../src/token.c"
#include "test.h"

void test_evaluate_num(void) {
  TokenError err1;
  EvaluateError err2;
  uint64_t idx;
  Token* tkn;
  Result result;

  idx = 0;
  tkn = parse("1", &idx, &err1);
  TEST_EQ(err1, TOKEN_ERROR_NIL);

  err2 = evalute(tkn, &result);
  TEST_EQ(err2, EVALUATE_ERROR_NIL);

  TEST_EQ(result.type, RESULT_NUM);
  TEST_EQ(result.value.num, 1);
}

int main(void) {
  ADD_TEST(test_evaluate_num);
  RUN_TESTS();
  return 0;
}
