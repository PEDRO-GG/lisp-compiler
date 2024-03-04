#include "../src/compiler.c"

#include "../src/array.c"
#include "../src/errors.c"
#include "../src/fatstr.c"
#include "../src/token.c"
#include "test.h"

bool compiler_cmp(Compiler* c1, Compiler* c2) {
  uint64_t idents_length1 = array_length(c1->idents);
  uint64_t idents_length2 = array_length(c2->idents);

  uint64_t scopes_length1 = array_length(c1->scopes);
  uint64_t scopes_length2 = array_length(c2->scopes);

  uint64_t code_length1 = array_length(c1->code);
  uint64_t code_length2 = array_length(c2->code);

  uint64_t errs_length1 = array_length(c1->errs);
  uint64_t errs_length2 = array_length(c2->errs);

  if (c1->stack != c2->stack || idents_length1 != idents_length2 ||
      scopes_length1 != scopes_length2 || code_length1 != code_length2 ||
      errs_length1 != errs_length2) {
    return false;
  }

  // Compare each identifer
  for (size_t i = 0; i < idents_length1; i++) {
    Identifier* ident1 = array_get(c1->idents, i);
    Identifier* ident2 = array_get(c2->idents, i);

    if (!ident_cmp(ident1, ident2)) {
      return false;
    }
  }

  // Compare each scope
  if (!array_cmp(c1->scopes, c2->scopes)) {
    return false;
  }

  // Compare each instruction
  for (size_t i = 0; i < code_length1; i++) {
    Instruction* inst1 = array_get(c1->code, i);
    Instruction* inst2 = array_get(c2->code, i);

    if (!instruction_cmp(inst1, inst2)) {
      return false;
    }
  }

  // Compare erros
  for (size_t i = 0; i < errs_length1; i++) {
    Error* err1 = array_get(c1->errs, i);
    Error* err2 = array_get(c2->errs, i);

    if (!errors_cmp(err1, err2) == 0) {
      return false;
    }
  }

  return true;
}

void run_compiler_test(const char* input, const char* expected,
                       Compiler* expected_state) {
  // Tokenize
  Parser parser = new_parser(input);
  Token* tkn = parse(&parser);

  // Compile
  Compiler cs = new_compiler();
  compile(&cs, tkn);

  // Convert instructions to string
  char buffer[100] = {0};
  for (size_t i = 0; i < array_length(cs.code); i++) {
    Instruction* inst = array_get(cs.code, i);
    instruction_to_string(inst, buffer);
  }

  // Test
  bool res = strcmp(buffer, expected) == 0;
  TEST_EQ(res, true);

  // Compare expected state with returned state;
  if (expected_state == NULL) {
    return;
  }
  res = compiler_cmp(&cs, expected_state);
  TEST_EQ(res, true);
}

void test_num(void) {
  run_compiler_test("10", "const 10 0\n", NULL);
  run_compiler_test("-10", "const -10 0\n", NULL);
  run_compiler_test("1000", "const 1000 0\n", NULL);
  run_compiler_test("---1000", "const -1000 0\n", NULL);
}

void test_var(void) {
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
      "(do"
      "    (var a 123)"
      ")",
      "const 123 0\n", NULL);
  // run_compiler_test(
  //     "(do "
  //     "    (var a 1)"
  //     "    (var b 2)"
  //     "    (var c 3)"
  //     "    (var d 4)"
  //     "    (do"
  //     "        (var e 5)"
  //     "        (var f 6)"
  //     "        (var g 7)"
  //     "        (var h 8)"
  //     "        (do"
  //     "            (var i 9)"
  //     "            (var j 10)"
  //     "            (var k 11)"
  //     "            (var l 12)"
  //     "        )"
  //     "    )"
  //     ")",
  //     "const 1 0\n"
  //     "const 2 1\n"
  //     "const 3 2\n"
  //     "const 4 3\n"
  //     "const 5 4\n"
  //     "const 6 5\n"
  //     "const 7 6\n"
  //     "const 8 7\n"
  //     "const 9 8\n"
  //     "const 10 9\n"
  //     "const 11 10\n"
  //     "const 12 11\n"
  //     "mov 11 8\n"
  //     "mov 8 4\n"
  //     "mov 4 0\n");
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
}

int main(void) {
  ADD_TEST(test_num);
  // ADD_TEST(test_var);
  ADD_TEST(test_binop);
  RUN_TESTS();
  return exit_code();
}
