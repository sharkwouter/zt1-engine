#ifndef PALLET_HPP
#define PALLET_HPP

#include <cstdint>

typedef struct {
  uint32_t color_count;
  uint32_t colors[256];
} Pallet;

#endif // PALLET_HPP