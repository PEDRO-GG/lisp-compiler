#include <assert.h>
#include <stdio.h>

#include "token.h"

int main(void) {
  Errors* errs = errors_init();
  const char* input = "  (  123  )  ";
  Parser parser = new_parser(input);
  Token* tkn = parse(&parser, errs);
  char buffer[100];
  token_to_string(tkn, buffer);
  printf("%s\n", buffer);
}
