#ifndef FATSTR_H
#define FATSTR_H

#include <stdint.h>

typedef struct FatStr {
  const uint8_t* start;
  uint64_t length;
} FatStr;

#endif  // FATSTR_H
