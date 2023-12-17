#ifndef TEST_H
#define TEST_H
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

typedef struct {
  const char* expr;
} FailedTest;

#define TEST_ASSERT(expr)                                          \
  do {                                                             \
    if (!(expr)) {                                                 \
      fprintf(stderr, "Assertion failed: %s\n", #expr);            \
      fprintf(stderr, "File: %s, Line: %d\n", __FILE__, __LINE__); \
      exit(EXIT_FAILURE);                                          \
    }                                                              \
  } while (0)

#endif  // TEST_H
