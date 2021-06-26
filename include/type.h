#ifndef _TYPE_H_
#define  _TYPE_H_

typedef enum {
    false,
    true
} bool_t;

typedef unsigned char         uint8_t;
typedef unsigned short        uint16_t;
typedef unsigned int          uint32_t;
typedef unsigned long long    uint64_t;
typedef char                  int8_t;
typedef short                 int16_t;
typedef int                   int32_t;
typedef long long             int64_t;

typedef unsigned long long    size_t;

#define NULL ((void*)0)

#endif
