#ifndef FATSTR_H
#define FATSTR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct FatStr {
  const uint8_t* start;
  uint64_t length;
} FatStr;

bool fatstr_cmp(const FatStr* f1, const FatStr* f2);
bool fatstr_to_str(const FatStr* f, char* buffer);

#endif  // FATSTR_H
