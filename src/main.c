#include <assert.h>
#include <stdio.h>

#include "token.h"

int main(void) {
  TokenError err;
  Token* list = token_list_make(&err);
  if (err != TOKEN_ERROR_NIL) {
    printf("Error!\n");
  }

  assert(list != NULL);

  Token t1 = {
      .type = TOKEN_NUM,
      .value.num = 100,
  };
  err = token_list_append(list, &t1);
  if (err != TOKEN_ERROR_NIL) {
    printf("Error!\n");
  }

  Token t2 = {
      .type = TOKEN_NUM,
      .value.num = 200,
  };
  err = token_list_append(list, &t2);
  if (err != TOKEN_ERROR_NIL) {
    printf("Error!\n");
  }

  char buffer[100];
  to_string(list, buffer);
  printf("%s\n", buffer);
}
