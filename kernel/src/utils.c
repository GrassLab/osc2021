#include "utils.h"

int is_digit(char ch) { return (ch >= '0') && (ch <= '9'); }

uint64_t hex2int(char *hex, int len) {
  uint64_t val = 0;
  for (int i = 0; i < len; i++) {
    // get current character then increment
    uint64_t byte = *(hex + i);
    if (byte >= '0' && byte <= '9')
      byte = byte - '0';
    else if (byte >= 'A' && byte <= 'F')
      byte = byte - 'A' + 10;
    else if (byte >= 'a' && byte <= 'f')
      byte = byte - 'a' + 10;

    val = (val << 4) | (byte & 0xF);
  }
  return val;
}

uint64_t align_up(uint64_t addr, uint64_t alignment) {
  return (addr + alignment - 1) & (~(alignment - 1));
}

uint32_t get_value32(uint64_t addr, char endian) {
  char *base = (char *)addr;
  uint32_t value = 0;
  if (endian == 'L' || endian == 'l') {
    for (int i = 3; i >= 0; i--) {
      value = (value << 4) | (*(base + i) & 0xFF);
    }
  }
  if (endian == 'B' || endian == 'b') {
    for (int i = 0; i < 4; i++) {
      value = (value << 8) | (*(base + i) & 0xFF);
    }
  }
  return value;
}

// big endian to little endian
uint32_t be2le(uint32_t x) {
  return (((x >> 24) & 0xFF) << 0) | (((x >> 16) & 0xFF) << 8) |
         (((x >> 8) & 0xFF) << 16) | (((x >> 0) & 0xFF) << 24);
}

uint64_t log2(uint64_t num) {
  for (uint64_t i = 0; i < 64; i++) {
    if (num == (1 << i)) return i;
  }
  return 0;
}
