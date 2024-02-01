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

  int* a_ptr = array_get(array, 0);
  int* b_ptr = array_get(array, 1);
  int* c_ptr = array_get(array, 2);
  int* d_ptr = array_get(array, 3);
  int* e_ptr = array_get(array, 4);
  int* f_ptr = array_get(array, 5);
  int* g_ptr = array_get(array, 6);
  int* h_ptr = array_get(array, 7);
  int* i_ptr = array_get(array, 8);
  int* j_ptr = array_get(array, 9);
  int* k_ptr = array_get(array, 10);
  int* l_ptr = array_get(array, 11);
  int* m_ptr = array_get(array, 12);
  int* n_ptr = array_get(array, 13);
  int* o_ptr = array_get(array, 14);
  int* p_ptr = array_get(array, 15);
  int* q_ptr = array_get(array, 16);
  int* r_ptr = array_get(array, 17);
  int* s_ptr = array_get(array, 18);
  int* t_ptr = array_get(array, 19);
  int* u_ptr = array_get(array, 20);
  int* v_ptr = array_get(array, 21);
  int* w_ptr = array_get(array, 22);
  int* x_ptr = array_get(array, 23);
  int* y_ptr = array_get(array, 24);
  int* z_ptr = array_get(array, 25);

  TEST_EQ((*a_ptr), 0);
  TEST_EQ((*b_ptr), 1);
  TEST_EQ((*c_ptr), 2);
  TEST_EQ((*d_ptr), 3);
  TEST_EQ((*e_ptr), 4);
  TEST_EQ((*f_ptr), 5);
  TEST_EQ((*g_ptr), 6);
  TEST_EQ((*h_ptr), 7);
  TEST_EQ((*i_ptr), 8);
  TEST_EQ((*j_ptr), 9);
  TEST_EQ((*k_ptr), 10);
  TEST_EQ((*l_ptr), 11);
  TEST_EQ((*m_ptr), 12);
  TEST_EQ((*n_ptr), 13);
  TEST_EQ((*o_ptr), 14);
  TEST_EQ((*p_ptr), 15);
  TEST_EQ((*q_ptr), 16);
  TEST_EQ((*r_ptr), 17);
  TEST_EQ((*s_ptr), 18);
  TEST_EQ((*t_ptr), 19);
  TEST_EQ((*u_ptr), 20);
  TEST_EQ((*v_ptr), 21);
  TEST_EQ((*w_ptr), 22);
  TEST_EQ((*x_ptr), 23);
  TEST_EQ((*y_ptr), 24);
  TEST_EQ((*z_ptr), 25);

  TEST_EQ(array->length, 26);
  TEST_EQ(array->capacity, 40);

  array_remove(array, 3);
  TEST_EQ(array->length, 25);
  TEST_EQ(array->capacity, 40);

  a_ptr = array_get(array, 0);
  b_ptr = array_get(array, 1);
  c_ptr = array_get(array, 2);
  d_ptr = array_get(array, 3);
  e_ptr = array_get(array, 4);
  f_ptr = array_get(array, 5);
  g_ptr = array_get(array, 6);
  h_ptr = array_get(array, 7);
  i_ptr = array_get(array, 8);
  j_ptr = array_get(array, 9);
  k_ptr = array_get(array, 10);
  l_ptr = array_get(array, 11);
  m_ptr = array_get(array, 12);
  n_ptr = array_get(array, 13);
  o_ptr = array_get(array, 14);
  p_ptr = array_get(array, 15);
  q_ptr = array_get(array, 16);
  r_ptr = array_get(array, 17);
  s_ptr = array_get(array, 18);
  t_ptr = array_get(array, 19);
  u_ptr = array_get(array, 20);
  v_ptr = array_get(array, 21);
  w_ptr = array_get(array, 22);
  x_ptr = array_get(array, 23);
  y_ptr = array_get(array, 24);
  z_ptr = array_get(array, 25);  // Out of bounds

  TEST_EQ((*a_ptr), 0);
  TEST_EQ((*b_ptr), 1);
  TEST_EQ((*c_ptr), 2);
  TEST_EQ((*d_ptr), 4);
  TEST_EQ((*e_ptr), 5);
  TEST_EQ((*f_ptr), 6);
  TEST_EQ((*g_ptr), 7);
  TEST_EQ((*h_ptr), 8);
  TEST_EQ((*i_ptr), 9);
  TEST_EQ((*j_ptr), 10);
  TEST_EQ((*k_ptr), 11);
  TEST_EQ((*l_ptr), 12);
  TEST_EQ((*m_ptr), 13);
  TEST_EQ((*n_ptr), 14);
  TEST_EQ((*o_ptr), 15);
  TEST_EQ((*p_ptr), 16);
  TEST_EQ((*q_ptr), 17);
  TEST_EQ((*r_ptr), 18);
  TEST_EQ((*s_ptr), 19);
  TEST_EQ((*t_ptr), 20);
  TEST_EQ((*u_ptr), 21);
  TEST_EQ((*v_ptr), 22);
  TEST_EQ((*w_ptr), 23);
  TEST_EQ((*x_ptr), 24);
  TEST_EQ((*y_ptr), 25);
  TEST_EQ_PTR(z_ptr, NULL);

  array_truncate(array, 5);  // Remove everything from idx 5 and onwards
  TEST_EQ(array->length, 5);
  TEST_EQ(array->capacity, 40);

  a_ptr = array_get(array, 0);
  b_ptr = array_get(array, 1);
  c_ptr = array_get(array, 2);
  d_ptr = array_get(array, 3);
  e_ptr = array_get(array, 4);
  f_ptr = array_get(array, 5);
  g_ptr = array_get(array, 6);
  h_ptr = array_get(array, 7);
  i_ptr = array_get(array, 8);
  j_ptr = array_get(array, 9);
  k_ptr = array_get(array, 10);
  l_ptr = array_get(array, 11);
  m_ptr = array_get(array, 12);
  n_ptr = array_get(array, 13);
  o_ptr = array_get(array, 14);
  p_ptr = array_get(array, 15);
  q_ptr = array_get(array, 16);
  r_ptr = array_get(array, 17);
  s_ptr = array_get(array, 18);
  t_ptr = array_get(array, 19);
  u_ptr = array_get(array, 20);
  v_ptr = array_get(array, 21);
  w_ptr = array_get(array, 22);
  x_ptr = array_get(array, 23);
  y_ptr = array_get(array, 24);
  z_ptr = array_get(array, 25);  // Out of bounds

  TEST_EQ((*a_ptr), 0);
  TEST_EQ((*b_ptr), 1);
  TEST_EQ((*c_ptr), 2);
  TEST_EQ((*d_ptr), 4);
  TEST_EQ((*e_ptr), 5);
  TEST_EQ_PTR(f_ptr, NULL);
  TEST_EQ_PTR(g_ptr, NULL);
  TEST_EQ_PTR(h_ptr, NULL);
  TEST_EQ_PTR(i_ptr, NULL);
  TEST_EQ_PTR(j_ptr, NULL);
  TEST_EQ_PTR(k_ptr, NULL);
  TEST_EQ_PTR(l_ptr, NULL);
  TEST_EQ_PTR(m_ptr, NULL);
  TEST_EQ_PTR(n_ptr, NULL);
  TEST_EQ_PTR(o_ptr, NULL);
  TEST_EQ_PTR(p_ptr, NULL);
  TEST_EQ_PTR(q_ptr, NULL);
  TEST_EQ_PTR(r_ptr, NULL);
  TEST_EQ_PTR(s_ptr, NULL);
  TEST_EQ_PTR(t_ptr, NULL);
  TEST_EQ_PTR(u_ptr, NULL);
  TEST_EQ_PTR(v_ptr, NULL);
  TEST_EQ_PTR(w_ptr, NULL);
  TEST_EQ_PTR(x_ptr, NULL);
  TEST_EQ_PTR(y_ptr, NULL);
  TEST_EQ_PTR(z_ptr, NULL);

  int* popped = array_pop(array);
  TEST_EQ(array->length, 4);
  TEST_EQ(array->capacity, 40);
  TEST_EQ((*popped), 5);
}

int main(void) {
  ADD_TEST(test_array);
  RUN_TESTS();
  return 0;
}
