#ifndef TEST_H
#define TEST_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef void (*TestFunc)(void);

typedef struct {
  const char* name;
  TestFunc func;
} Test;

void add_test(Test t);

#define ADD_TEST(test_func) \
  do {                      \
    add_test((Test){        \
        .name = #test_func, \
        .func = test_func,  \
    });                     \
  } while (0)

void run_tests(const char* file_name);

#define RUN_TESTS()      \
  do {                   \
    run_tests(__FILE__); \
  } while (0)

void test_assert(bool expr, const char* raw_expr, const char* file_name,
                 int line_num);

#define TEST_ASSERT(expr)                         \
  do {                                            \
    test_assert(expr, #expr, __FILE__, __LINE__); \
  } while (0)

#endif  // TEST_H
