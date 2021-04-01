#ifndef STRING_H
#define STRING_H

#include <stdint.h>

class String {
public:
    static bool Equal(const char* str1, const char* str2);
    static char* ToHex(const uint32_t val);
};

#endif
