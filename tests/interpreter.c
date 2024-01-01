#include "../src/interpreter.c"

#include "../src/fatstr.c"
#include "../src/token.c"
#include "test.h"

void run_evaluate_test(const char* input, const Result* expected_result) {
  TokenError err1;
  EvaluateError err2;
  uint64_t idx;
  Token* tkn;
  Result result;

  idx = 0;
  tkn = parse(input, &idx, &err1);
  TEST_EQ(err1, TOKEN_ERROR_NIL);

  err2 = evalute(tkn, &result);
  TEST_EQ(err2, EVALUATE_ERROR_NIL);
  TEST_EQ(rescmp(&result, expected_result), true);
}

void test_evaluate_num(void) {
  run_evaluate_test("1", &(Result){
                             .type = RESULT_NUM,
                             .value.num = 1,
                         });
}

void test_evaluate_bool(void) {
  run_evaluate_test("true", &(Result){
                                .type = RESULT_BOOL,
                                .value.boolean = true,
                            });
}

void test_evaluate_arithmetic(void) {
  run_evaluate_test("(+ 1 2)", &(Result){
                                   .type = RESULT_NUM,
                                   .value.num = 3,
                               });
  run_evaluate_test("(- 1 2)", &(Result){
                                   .type = RESULT_NUM,
                                   .value.num = -1,
                               });
  run_evaluate_test("(/ 2 2)", &(Result){
                                   .type = RESULT_NUM,
                                   .value.num = 1,
                               });
  run_evaluate_test("(* 2 2)", &(Result){
                                   .type = RESULT_NUM,
                                   .value.num = 4,
                               });
}

void test_env_append(void) {
  Var var1 = (Var){
      .name =
          (FatStr){
              .start = (const uint8_t*)"a",
              .length = 1,
          },
      .result =
          (Result){
              .type = RESULT_NUM,
              .value.num = 1,
          },
  };

  Var var2 = (Var){
      .name =
          (FatStr){
              .start = (const uint8_t*)"b",
              .length = 1,
          },
      .result =
          (Result){
              .type = RESULT_NUM,
              .value.num = 2,
          },
  };

  EvaluateError err;
  Env* env = env_make(&err, NULL);
  TEST_EQ(err, EVALUATE_ERROR_NIL);

  err = env_append(env, var1);
  TEST_EQ(err, EVALUATE_ERROR_NIL);

  err = env_append(env, var2);
  TEST_EQ(err, EVALUATE_ERROR_NIL);

  TEST_EQ(varcmp(&var1, &env->data[0]), true);
  TEST_EQ(varcmp(&var2, &env->data[1]), true);
}

int main(void) {
  ADD_TEST(test_evaluate_num);
  ADD_TEST(test_evaluate_bool);
  ADD_TEST(test_evaluate_arithmetic);
  ADD_TEST(test_env_append);
  RUN_TESTS();
  return 0;
}
