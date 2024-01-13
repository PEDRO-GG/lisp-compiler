#include "fatstr.h"

bool fatstr_cmp(const FatStr* f1, const FatStr* f2) {
  if (f1->length != f2->length) return false;

  for (uint64_t i = 0; i < f1->length; i++) {
    if (f1->start[i] != f2->start[i]) return false;
  }

  return true;
}

bool fatstr_to_str(const FatStr* f, char* buffer) {
  if (buffer == NULL) return false;
  strncat(buffer, (char*)f->start, f->length);
  return true;
}
