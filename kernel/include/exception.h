#pragma once

#include <stdint.h>

#include "gpio.h"

#define CORE0_IRQ_SOURCE ((volatile unsigned int *)(0x40000060))

#define GPU_IRQ (1 << 8)
#define CNTPNS_IRQ (1 << 1)

#define SYS_UART_READ 0
#define SYS_UART_WRITE 1
#define SYS_GETPID 2
#define SYS_FORK 3
#define SYS_EXEC 4
#define SYS_EXIT 5
#define SYS_OPEN 6
#define SYS_CLOSE 7
#define SYS_WRITE 8
#define SYS_READ 9

void sync_handler_currentEL_ELx();
void sync_handler_lowerEL_64(uint64_t sp_addr);
void irq_handler_currentEL_ELx();
void irq_handler_lowerEL_64();
void default_handler();
void enable_interrupt();
void disable_interrupt();
