#include "../src/compiler.c"

#include "../src/array.c"
#include "../src/errors.c"
#include "../src/fatstr.c"
#include "../src/token.c"
#include "test.h"

void run_compiler_test(const char* input, const char* expected) {
  Parser parser = new_parser(input);
  Token* tkn = parse(&parser);
  Compiler cs = new_compiler();
  compile(&cs, tkn);
  char buffer[100] = {0};
  for (size_t i = 0; i < array_length(cs.code); i++) {
    Instruction* inst = array_get(cs.code, i);
    instruction_to_string(inst, buffer);
  }

  bool res = strcmp(buffer, expected) == 0;
  TEST_EQ(res, true);
}

void test_num(void) {
  run_compiler_test("10", "const 10 0\n");
  run_compiler_test("-10", "const -10 0\n");
  run_compiler_test("1000", "const 1000 0\n");
  run_compiler_test("---1000", "const -1000 0\n");
}

void test_var(void) {
  run_compiler_test(
      "(do"
      "    (var a 123)"
      ")",
      "const 123 0\n");
  run_compiler_test(
      "(do "
      "    (var a 1)"
      "    (var b 2)"
      "    (var c 3)"
      ")",
      "const 1 0\n"
      "const 2 1\n"
      "const 3 2\n");
}

void test_binop(void) {
  run_compiler_test("(- 1 2)",
                    "const 1 0\n"
                    "const 2 1\n"
                    "binop - 0 1 0\n");
  run_compiler_test("(+ 100 200)",
                    "const 100 0\n"
                    "const 200 1\n"
                    "binop + 0 1 0\n");
}

int main(void) {
  ADD_TEST(test_num);
  ADD_TEST(test_binop);
  ADD_TEST(test_var);
  RUN_TESTS();
  return exit_code();
}
