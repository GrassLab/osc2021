#include "include/uart.h"
#include "include/util.h"
#include "include/scheduler.h"

void invalid_synchronous_handler() {
    print_register(current->pid);
    uart_put_str("no handle this synchronous exception.\n");
    while(1);
}

void invalid_irq_handler() {
    print_register(current->pid);
    uart_put_str("no handle this IRQ exception.\n");
    while(1);
}

void invalid_fiq_handler() {
   print_register(current->pid);
    uart_put_str("no handle this FIQ exception.\n");
    while(1);
}

void invalid_serror_handler() {
    print_register(current->pid);
    uart_put_str("no handle this SError exception.\n");
    while(1);
}