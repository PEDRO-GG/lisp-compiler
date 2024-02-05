#include "../src/compiler.c"

#include "../src/array.c"
#include "../src/errors.c"
#include "../src/token.c"
#include "test.h"

void test_num(void) {
  Errors* errs = errors_init();
  Parser parser = new_parser("10");
  Token* tkn = parse(&parser, errs);
  Compiler cs = new_compiler(tkn);
  compile(&cs);
  bool res = array_compare_with_string(cs.code, "const 10 0");
  TEST_EQ(res, true);
}

int main(void) {
  ADD_TEST(test_num);
  RUN_TESTS();
  return 0;
}
