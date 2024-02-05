#include <assert.h>
#include <stdio.h>

#include "token.h"

int main(void) {
  Array* errs = array_new(10, sizeof(Error));
  const char* input = "  (  123  )  ";
  Parser parser = new_parser(input);
  Token* tkn = parse(&parser, errs);
  char buffer[100];
  token_to_string(tkn, buffer);
  printf("%s\n", buffer);
}
