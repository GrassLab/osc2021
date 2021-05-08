#pragma once

typedef unsigned long long int uint64_t;
static_assert(sizeof(uint64_t) == 8);
typedef signed   long long int int64_t;
static_assert(sizeof(int64_t) == 8);

typedef unsigned int uint32_t;
static_assert(sizeof(uint32_t) == 4);
typedef signed   int int32_t;
static_assert(sizeof(int32_t) == 4);

typedef unsigned short int uint16_t;
static_assert(sizeof(uint16_t) == 2);
typedef signed   short int int16_t;
static_assert(sizeof(int16_t) == 2);

typedef unsigned char uint8_t;
static_assert(sizeof(uint8_t) == 1);
typedef signed   char int8_t;
static_assert(sizeof(int8_t) == 1);

typedef int64_t size_t;
