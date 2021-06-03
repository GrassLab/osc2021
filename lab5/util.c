#include "include/util.h"
#include "include/uart.h"

unsigned int big_to_little_32(unsigned int b_num) {
    unsigned int l_num = 0;
    for (int i = 0; i < 4; i++) {
        l_num = l_num << 8;
        l_num += b_num & 0xff;
        b_num = b_num >> 8;
    }
    return l_num;
}

unsigned long big_to_little_64(unsigned long b_num) {
    unsigned int l_num = 0;
    for (int i = 0; i < 8; i++) {
        l_num = l_num << 8;
        l_num += b_num & 0xff;
        b_num = b_num >> 8;
    }
    return l_num;
}

unsigned long align(unsigned long num, int size) {
    if (num == 0)
        return 0;
    return (((num - 1) / size) + 1) * size;
}

void set_time() {
    asm volatile (
        "mrs x10, cntfrq_el0\n\t"
        "mov x10, x10, lsl 1\n\t"
        "msr cntp_tval_el0, x10\n\t"
        :
        :
        :
    );
}

unsigned long get_time() {
    unsigned long timer_count, timer_freq = 0;
    asm volatile (
        "mrs %[timer_freq], cntfrq_el0\n\t"
        "mrs %[timer_count], cntpct_el0\n\t"
        : [timer_count] "=r" (timer_count), [timer_freq] "=r" (timer_freq)
        :
        :
    );
    unsigned long time = timer_count / timer_freq;
    return time;
}

void print_register(int pid) {
    unsigned long elr_el1, esr_el1, x16, spsr_el1, x17, x15;
    asm volatile ("mrs %[elr_el1], elr_el1\n\t": [elr_el1] "=r" (elr_el1)::);
    asm volatile ("mrs %[esr_el1], esr_el1\n\t": [esr_el1] "=r" (esr_el1)::);
    asm volatile ("mrs %[spsr_el1], spsr_el1\n\t": [spsr_el1] "=r" (spsr_el1)::);
    //asm volatile ("mrs %[cpsr], cpsr\n\t": [cpsr] "=r" (cpsr)::);
    asm volatile ("mov %[x16], x16\n\t": [x16] "=r" (x16)::);
    asm volatile ("mov %[x17], x17\n\t": [x17] "=r" (x17)::);
    asm volatile ("mov %[x15], x15\n\t": [x15] "=r" (x15)::);
    uart_put_str("pid: ");
    uart_put_int(pid);
    uart_put_str("\n");
    uart_put_str("elr_el1: ");
    uart_put_addr(elr_el1);
    uart_put_str("\nesr_el1: ");
    uart_put_addr(esr_el1);
    uart_put_str("\nspsr_el1: ");
    uart_put_addr(spsr_el1);
    //uart_put_str("\ncpsr: ");
    //uart_put_addr(cpsr);
    uart_put_str("\nlr: ");
    uart_put_addr(x16);
    uart_put_str("\nsp: ");
    uart_put_addr(x17);
    uart_put_str("\nx15: ");
    uart_put_addr(x15);
    uart_put_str("\n");
    
}