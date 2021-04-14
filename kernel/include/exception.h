#pragma once

#include "gpio.h"

void sync_handler();
void irq_handler_currentEL_ELx();
void irq_handler_lowerEL_64();
void default_handler();
void enable_interrupt();
void disable_interrupt();
