#include "../src/errors.c"

#include "../src/array.c"
#include "test.h"

void test_error_append(void) {
  Array* errs = array_new(10, sizeof(Error));
  TEST_NOT_EQ_PTR(errs, NULL);
  TEST_EQ(errs->capacity, 10);
  TEST_EQ(errs->length, 0);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 1);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 2);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 3);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 4);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 5);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 6);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 7);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 8);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 9);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 10);
  TEST_EQ(errs->capacity, 10);

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 11);
  TEST_EQ(errs->capacity, (2 * 10));

  array_append(errs, &(Error){0});
  TEST_EQ(errs->length, 12);
  TEST_EQ(errs->capacity, (2 * 10));
}

int main(void) {
  ADD_TEST(test_error_append);
  RUN_TESTS();
  return 0;
}
