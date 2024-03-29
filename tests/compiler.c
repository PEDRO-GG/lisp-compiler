#include "../src/compiler.c"

#include "../src/array.c"
#include "../src/errors.c"
#include "../src/fatstr.c"
#include "../src/token.c"
#include "test.h"

void run_compiler_test(const char* input, const char* expected,
                       Compiler* expected_state) {
  // Tokenize
  Parser parser = new_parser(input);
  Token* tkn = parse(&parser);

  // Compile
  Compiler cs = new_compiler();
  compile_expr(&cs, tkn, false);

  // Test
  Array* buffer = dump_ir(&cs);
  TEST_STRCMP(array_to_str(buffer), expected);

  // Compare expected state with returned state;
  if (expected_state == NULL) {
    return;
  }
  bool res = compiler_cmp(&cs, expected_state);
  TEST_EQ(res, true);
}

void test_num(void) {
  run_compiler_test("10", "const 10 0\n", NULL);
  run_compiler_test("-10", "const -10 0\n", NULL);
  run_compiler_test("1000", "const 1000 0\n", NULL);
  run_compiler_test("---1000", "const -1000 0\n", NULL);
  run_compiler_test("(do 1 2)", "const 1 0\nconst 2 0\n", NULL);
}

void test_var(void) {
  run_compiler_test(
      "(do"
      "    (var a 123)"
      ")",
      "const 123 0\n", NULL);

  run_compiler_test(
      "(do "
      "  (var a 100)"
      "  (var b 200)"
      "  (var c 300)"
      ")",
      "const 100 0\n"
      "const 200 1\n"
      "const 300 2\n"
      "mov 2 0\n",
      NULL);

  run_compiler_test(
      "(do "
      "    (var a 1)"
      "    (var b 2)"
      "    (var c 3)"
      "    (var d 4)"
      "    (do"
      "        (var e 5)"
      "        (var f 6)"
      "        (var g 7)"
      "        (var h 8)"
      "        (do"
      "            (var i 9)"
      "            (var j 10)"
      "            (var k 11)"
      "            (var l 12)"
      "        )"
      "    )"
      ")",
      "const 1 0\n"
      "const 2 1\n"
      "const 3 2\n"
      "const 4 3\n"
      "const 5 4\n"
      "const 6 5\n"
      "const 7 6\n"
      "const 8 7\n"
      "const 9 8\n"
      "const 10 9\n"
      "const 11 10\n"
      "const 12 11\n"
      "mov 11 8\n"
      "mov 8 4\n"
      "mov 4 0\n",
      NULL);
}

void test_set(void) {
  run_compiler_test(
      "(do "
      "  (var a 100)"
      "  (var b 200)"
      "  (var c -1)"
      "  (var d -1)"
      "  (set c a)"
      "  (set d b)"
      ")",
      "const 100 0\n"
      "const 200 1\n"
      "const -1 2\n"
      "const -1 3\n"
      "mov 0 2\n"
      "mov 1 3\n"
      "mov 3 0\n",
      NULL);
}

void test_binop(void) {
  run_compiler_test("(- 1 2)",
                    "const 1 0\n"
                    "const 2 1\n"
                    "binop - 0 1 0\n",
                    NULL);
  run_compiler_test("(+ 100 200)",
                    "const 100 0\n"
                    "const 200 1\n"
                    "binop + 0 1 0\n",
                    NULL);
  run_compiler_test("(+ (- 1 2) 3)",
                    "const 1 0\n"
                    "const 2 1\n"
                    "binop - 0 1 0\n"
                    "const 3 1\n"
                    "binop + 0 1 0\n",
                    NULL);
  run_compiler_test("(do (var a 1) (+ 3 a))",
                    "const 1 0\n"
                    "const 3 1\n"
                    "binop + 1 0 1\n"
                    "mov 1 0\n",
                    NULL);
  run_compiler_test("(do (var a 1) (set a (+ 3 a)) (var b 2) (- b a))",
                    "const 1 0\n"
                    "const 3 1\n"
                    "binop + 1 0 1\n"
                    "mov 1 0\n"
                    "const 2 1\n"
                    "binop - 1 0 2\n"
                    "mov 2 0\n",
                    NULL);
  run_compiler_test("(do (var a 1) (do (var b (+ a 2)) (set a (+ b 3))) )",
                    "const 1 0\n"
                    "const 2 1\n"
                    "binop + 0 1 1\n"
                    "const 3 2\n"
                    "binop + 1 2 2\n"
                    "mov 2 0\n",
                    NULL);
}

void test_if(void) {
  run_compiler_test(
      "(do "
      "    (var a 100) "
      "    (if (eq a 200) (set a 300) (set a 400))"
      "    a"
      ")",
      "const 100 0\n"
      "const 200 1\n"
      "binop eq 0 1 1\n"
      "jmpf 1 L1\n"
      "const 300 1\n"
      "mov 1 0\n"
      "mov 0 1\n"
      "jmp L0\n"
      "L1:\n"
      "const 400 1\n"
      "mov 1 0\n"
      "mov 0 1\n"
      "L0:\n",
      NULL);
}

void test_loop(void) {
  run_compiler_test(
      "(do "
      "    (var a 10) "
      "    (loop "
      "        (gt a 0) "
      "        (set a (- a 1))"
      "    )"
      ")",
      "const 10 0\n"
      "L0:\n"
      "const 0 1\n"
      "binop gt 0 1 1\n"
      "jmpf 1 L1\n"
      "const 1 1\n"
      "binop - 0 1 1\n"
      "mov 1 0\n"
      "jmp L0\n"
      "L1:\n",
      NULL);
}

int main(void) {
  ADD_TEST(test_num);
  ADD_TEST(test_var);
  ADD_TEST(test_set);
  ADD_TEST(test_binop);
  ADD_TEST(test_if);
  ADD_TEST(test_loop);
  RUN_TESTS();
  return exit_code();
}
