#pragma once

#include "types.h"

class MiniUART {
public:
    static void Init();
    static size_t GetS(char* str, uint64_t count);
    static void PutS(const char* str);
    static void PutS(const char* str, uint64_t length);
    static void PutUInt64(uint64_t val);
    static uint8_t GetCh();
    static void Send(uint8_t ch);
    static uint8_t Recv();
};

class io {
public:
    io operator<<(uint64_t val) {
        MiniUART::PutUInt64(val);
        return *this;
    }
    io operator<<(const char* str) {
        MiniUART::PutS(str);
        return *this;
    }
};
