  
#ifndef MINI_UART_H
#define MINI_UART_H
#include <stdint.h>

class MiniUART {
public:
    static void Init();
    static void GetS(char* str);
    static void PutS(const char* str);
    static void PutS(const char* str, uint64_t length);
    static void PutUInt64(uint64_t val);
    static uint8_t GetCh();
    static void Send(uint8_t ch);
    static uint8_t Recv();
};

class IO {
public:
    IO operator<<(uint64_t val) {
        MiniUART::PutUInt64(val);
        return *this;
    }
    IO operator<<(const char* str) {
        MiniUART::PutS(str);
        return *this;
    }
};

#endif
