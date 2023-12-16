#include <assert.h>
#include <stdio.h>

#include "token.h"

int main(void) {
  TokenError err;
  const char* input = "  (  123  )  ";
  uint64_t idx = 0;
  Token* tkn = parse(input, &idx, &err);
  if (err != TOKEN_ERROR_NIL) {
    printf("Error!\n");
  }

  char buffer[100];
  to_string(tkn, buffer);
  printf("%s\n", buffer);
}
