#include "uart.h"

extern unsigned char __begin, __begin_loader, __end_loader, __relocate_address;

__attribute__((section(".text.loader")))
void loader() {
    // load size
    char c;
    int img_size = 0;
    asm("nop");
    c = uart_read_raw();
    while(c!='\r') {
        img_size = img_size*10 + (c-'0');
        c = uart_read_raw();
    }
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    // load img
    unsigned char* kernel_address = (unsigned char*)&__begin;
    asm("ldr x0, =__begin");
    asm("str x0, [sp, #24]");

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    for(int i=0;i<img_size;i++) {
        c = uart_read_raw();
        *(kernel_address+i) = c;
    }

    void (*os_entry)(void) = (void*)&__begin;
    asm("ldr x0, =__begin");
    asm("str x0, [sp, #16]");

    os_entry();
}

void loadimg() {
    unsigned long loader_size = (&__end_loader - &__begin_loader);
    unsigned char *old_addr = (unsigned char*) &__begin_loader;
    unsigned char *new_addr = (unsigned char*) &__relocate_address;

    while(loader_size--) {
        *new_addr = *old_addr;
        new_addr++;
        old_addr++;
    }

    void (*loader_func)(void) = (void*)(&__relocate_address);

    loader_func();
}
