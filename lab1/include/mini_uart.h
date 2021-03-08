#ifndef MINI_UART_H
#define MINI_UART_H
#include <stdint.h>

class MiniUART {
public:
    static void Init();
    static void GetS(char* str);
    static void PutS(const char* str);
    static uint8_t GetCh();
private:
    static void Send(uint8_t ch);
    static uint8_t Recv();
};

#endif
