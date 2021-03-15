#ifndef STDINT_H
#define STDINT_H

typedef unsigned long long int uint64_t;
typedef signed long long int int64_t;
_Static_assert(sizeof(uint64_t) == 8);
_Static_assert(sizeof(int64_t) == 8);

typedef unsigned int uint32_t;
typedef signed int int32_t;
_Static_assert(sizeof(uint32_t) == 4);
_Static_assert(sizeof(int32_t) == 4);

typedef unsigned short int uint16_t;
typedef signed short int int16_t;
_Static_assert(sizeof(uint16_t) == 2);
_Static_assert(sizeof(int16_t) == 2);

typedef unsigned char uint8_t;
typedef signed char int8_t;
_Static_assert(sizeof(uint8_t) == 1);
_Static_assert(sizeof(int8_t) == 1);

#endif