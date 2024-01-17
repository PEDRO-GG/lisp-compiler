#include <assert.h>
#include <stdio.h>

#include "token.h"

int main(void) {
  Errors* errs = errors_init();
  const char* input = "  (  123  )  ";
  uint64_t idx = 0;
  Token* tkn = parse(input, &idx, errs);
  char buffer[100];
  token_to_string(tkn, buffer);
  printf("%s\n", buffer);
}
