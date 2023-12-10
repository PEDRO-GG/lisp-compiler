#include <stdio.h>

#include "token.h"

int main(void) {
  Token t = {.type = TOKEN_NUM, .value.num = 5.};
  printf("type: %d, value: %lld\n", t.type, t.value.num);
}
