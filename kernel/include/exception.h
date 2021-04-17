#pragma once

#include "gpio.h"

#define CORE0_IRQ_SOURCE ((volatile unsigned int *)(0x40000060))

#define GPU_IRQ (1 << 8)
#define CNTPNS_IRQ (1 << 1)

void sync_handler();
void irq_handler_currentEL_ELx();
void irq_handler_lowerEL_64();
void default_handler();
void enable_interrupt();
void disable_interrupt();
