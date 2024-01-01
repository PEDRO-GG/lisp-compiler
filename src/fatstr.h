#ifndef FATSTR_H
#define FATSTR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct FatStr {
  const uint8_t* start;
  uint64_t length;
} FatStr;

bool fatstr_cmp(const FatStr* f1, const FatStr* f2);

#endif  // FATSTR_H
