#pragma once

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

int is_digit(char ch);
unsigned long long int hex2int(char *hex, int len);
unsigned long long align_up(unsigned long long addr,
                            unsigned long long alignment);
uint32_t get_value32(uint64_t addr, char endian);
uint32_t be2le(uint32_t x);
