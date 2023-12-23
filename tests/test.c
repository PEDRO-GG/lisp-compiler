#include "test.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define ADDEND 10
#define MAX_MSGS 30
#define MAX_MSG_LENGTH 200

static uint64_t length = 0;
static uint64_t capacity = 0;
static Test* tests = NULL;

static uint64_t idx = 0;
static char fail_msgs[MAX_MSGS][MAX_MSG_LENGTH] = {0};

void add_test(Test t) {
  // Check if this is the very first test
  if (tests == NULL) {
    tests = malloc(sizeof(Test) * ADDEND);
    if (tests == NULL) {
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }
    capacity = ADDEND;
  } else if (length >= capacity) {
    // Request more memory if needed
    Test* tmp = realloc(tests, sizeof(Test) * ADDEND);
    if (tmp == NULL) {
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }
    tests = tmp;
    capacity += ADDEND;
  }

  tests[length++] = t;
}

void populate_fail_msg(const char* format, ...) {
  if (idx >= MAX_MSGS || idx < 0) {
    return;  // Index out of bounds
  }

  va_list args;
  va_start(args, format);
  vsnprintf(fail_msgs[idx++], MAX_MSG_LENGTH, format, args);
  va_end(args);
}

void print_fail_msgs(void) {
  for (int i = 0; i < MAX_MSGS; ++i) {
    if (fail_msgs[i][0] != '\0') {
      printf("    %s\n", fail_msgs[i]);
    }
  }
}

void reset_fail_msgs(void) { idx = 0; }

void run_tests(const char* file_name) {
  assert(tests != NULL);
  assert(length > 0);

  printf("File: %s\n", file_name);
  printf("  Running %llu ", length);
  if (length > 1) {
    printf("tests\n");
  } else {
    printf("test\n");
  }

  for (uint64_t i = 0; i < length; i++) {
    Test t = tests[i];
    printf("    [%llu/%llu] %s ... ", i + 1, length, t.name);
    t.func();
    if (fail_msgs[0][0] != '\0') {
      printf("FAIL\n");
      print_fail_msgs();
      reset_fail_msgs();
      printf("\n");
    } else {
      printf("OK\n");
    }
  }
}

void test_eq(const char* raw_left, const char* raw_right, int left, int right,
             const char* file_name, int line_num) {
  if (left != right) {
    populate_fail_msg(
        "%s:%d: assertion failed: %s != %s\n      left: %d\n      right: %d",
        file_name, line_num, raw_left, raw_right, left, right);
  }
}

void test_strcmp(const char* raw_left, const char* raw_right, const char* left,
                 const char* right, const char* file_name, int line_num) {
  if (strcmp(left, right) != 0) {
    populate_fail_msg(
        "%s:%d: assertion failed: %s != %s\n      left: \"%s\"\n      right: "
        "\"%s\"",
        file_name, line_num, raw_left, raw_right, left, right);
  }
}
