#include "../src/errors.c"

#include "test.h"

void test_error_append(void) {
  Errors* errs = errors_init();
  TEST_NOT_EQ_PTR(errs, NULL);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);
  TEST_EQ(errs->length, 0);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 1);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 2);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 3);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 4);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 5);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 6);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 7);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 8);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 9);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 10);
  TEST_EQ(errs->capacity, DEFAULT_CAPACITY);

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 11);
  TEST_EQ(errs->capacity, (2 * DEFAULT_CAPACITY));

  errors_append(errs, (Error){0});
  TEST_EQ(errs->length, 12);
  TEST_EQ(errs->capacity, (2 * DEFAULT_CAPACITY));
}

int main(void) {
  ADD_TEST(test_error_append);
  RUN_TESTS();
  return 0;
}
