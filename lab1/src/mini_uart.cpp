#include <mini_uart.h>

#include <mmio.h>
#include <kernel.h>

void MiniUART::Init() {
    uint32_t gpfsel1_value = MMIO::get(MMIOREG::GPFSEL1);
    gpfsel1_value = gpfsel1_value & ~((7 << 12) | (7 << 15)) | ((2 << 12) | (2 << 15));
    MMIO::set(MMIOREG::GPFSEL1, gpfsel1_value);
    MMIO::set(MMIOREG::GPPUD, 0);
    Kernel::Delay(150);
    MMIO::set(MMIOREG::GPPUDCLK0, (1 << 14) | (1 << 15));
    Kernel::Delay(150);
    MMIO::set(MMIOREG::GPPUD, 0);
    MMIO::set(MMIOREG::GPPUDCLK0, 0);

    MMIO::set(MMIOREG::AUX_ENABLES, 1);
    MMIO::set(MMIOREG::AUX_MU_CNTL_REG, 0);
    MMIO::set(MMIOREG::AUX_MU_IER_REG, 0);
    MMIO::set(MMIOREG::AUX_MU_LCR_REG, 3);
    MMIO::set(MMIOREG::AUX_MU_MCR_REG, 0);
    MMIO::set(MMIOREG::AUX_MU_BAUD_REG, 270);
    MMIO::set(MMIOREG::AUX_MU_IIR_REG, 6);
    MMIO::set(MMIOREG::AUX_MU_CNTL_REG, 3);
}

void MiniUART::Send(uint8_t ch) {
    while(!(MMIO::get(MMIOREG::AUX_MU_LSR_REG) & 0x20));
    MMIO::set(MMIOREG::AUX_MU_IO_REG, ch);
}

uint8_t MiniUART::Recv() {
    while(!(MMIO::get(MMIOREG::AUX_MU_LSR_REG) & 1));
    return MMIO::get(MMIOREG::AUX_MU_IO_REG);
}

uint8_t MiniUART::GetCh() {
    char ch = Recv();
    Send(ch);
    if (ch == '\r') Send('\n'); // When ENTER(\r) clicked, send \r\n
    return ch;
}

void MiniUART::GetS(char* str) {
    while (true) {
        uint8_t ch = GetCh();
        if (ch == '\r')
        {
            *str = 0;
            return;
        }
        else {
            *str = ch;
            str++;
        }
    }
}

void MiniUART::PutS(const char* str) {
    while (*str != 0) {
        Send(*str);
        str++;
    }
}
