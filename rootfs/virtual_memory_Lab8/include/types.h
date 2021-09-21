#ifndef _KERNEL_TYPES_H
#define _KERNEL_TYPES_H

#ifndef __ASSEMBLER__

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long int int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;

typedef unsigned int size_t;

typedef long pid_t; // task pid

#ifndef NULL
#define NULL ((void *)0)
#endif

#define TRUE 1
#define FALSE 0

#define EOF -1

#endif /* __ASSEMBLER__ */
#endif