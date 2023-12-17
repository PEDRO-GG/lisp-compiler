#include "test.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define ADDEND 10
static uint64_t length = 0;
static uint64_t capacity = 0;
static Test* tests = NULL;

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
    printf("OK\n");
  }
}
