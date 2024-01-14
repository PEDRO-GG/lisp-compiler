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

void test_eq(const char* raw_left, const char* raw_right, int left, int right,
             const char* file_name, int line_num);

#define TEST_EQ(left, right)                                 \
  do {                                                       \
    test_eq(#left, #right, left, right, __FILE__, __LINE__); \
  } while (0)

void test_not_eq(const char* raw_left, const char* raw_right, int left,
                 int right, const char* file_name, int line_num);

#define TEST_NOT_EQ(left, right)                                 \
  do {                                                           \
    test_not_eq(#left, #right, left, right, __FILE__, __LINE__); \
  } while (0)

void test_strcmp(const char* raw_left, const char* raw_right, const char* left,
                 const char* right, const char* file_name, int line_num);

#define TEST_STRCMP(left, right)                                 \
  do {                                                           \
    test_strcmp(#left, #right, left, right, __FILE__, __LINE__); \
  } while (0)

void test_eq_ptr(const char* raw_left, const char* raw_right, void* left,
                 void* right, const char* file_name, int line_num);

#define TEST_EQ_PTR(left, right)                                 \
  do {                                                           \
    test_eq_ptr(#left, #right, left, right, __FILE__, __LINE__); \
  } while (0)

void test_not_eq_ptr(const char* raw_left, const char* raw_right, void* left,
                     void* right, const char* file_name, int line_num);

#define TEST_NOT_EQ_PTR(left, right)                                 \
  do {                                                               \
    test_not_eq_ptr(#left, #right, left, right, __FILE__, __LINE__); \
  } while (0)

#endif  // TEST_H
