#include "../src/array.c"

#include "test.h"

void test_array(void) {
  Array* array = array_new(10, sizeof(int));
  TEST_EQ(array->length, 0);

  int a = 0;
  int b = 1;
  int c = 2;
  int d = 3;
  int e = 4;
  int f = 5;
  int g = 6;
  int h = 7;
  int i = 8;
  int j = 9;
  int k = 10;
  int l = 11;
  int m = 12;
  int n = 13;
  int o = 14;
  int p = 15;
  int q = 16;
  int r = 17;
  int s = 18;
  int t = 19;
  int u = 20;
  int v = 21;
  int w = 22;
  int x = 23;
  int y = 24;
  int z = 25;

  array_append(array, &a);
  array_append(array, &b);
  array_append(array, &c);
  array_append(array, &d);
  array_append(array, &e);
  array_append(array, &f);
  array_append(array, &g);
  array_append(array, &h);
  array_append(array, &i);
  array_append(array, &j);
  array_append(array, &k);
  array_append(array, &l);
  array_append(array, &m);
  array_append(array, &n);
  array_append(array, &o);
  array_append(array, &p);
  array_append(array, &q);
  array_append(array, &r);
  array_append(array, &s);
  array_append(array, &t);
  array_append(array, &u);
  array_append(array, &v);
  array_append(array, &w);
  array_append(array, &x);
  array_append(array, &y);
  array_append(array, &z);

  TEST_EQ(array->length, 26);
  TEST_EQ(array->capacity, 40);
}

int main(void) {
  ADD_TEST(test_array);
  RUN_TESTS();
  return 0;
}
