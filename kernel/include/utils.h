#pragma once

#include <stdint.h>

#define kb ((uint64_t)0x400)
#define mb ((uint64_t)0x100000)
#define gb ((uint64_t)0x40000000)

int is_digit(char ch);
uint64_t hex2int(char *hex, int len);
uint64_t align_up(uint64_t addr, uint64_t alignment);
uint64_t align_up_exp(uint64_t n);
uint32_t get_value32(uint64_t addr, char endian);
uint32_t be2le(uint32_t x);

uint64_t log2(uint64_t num);
void delay(int num);