#include <stdio.h>

#include "token.h"

int main(void) {
  Token t = {.type = TOKEN_NUM, .value.num = 500};
  char buffer[100];
  to_string(&t, buffer);
  printf("%s\n", buffer);
}
