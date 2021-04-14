#pragma once

#include "gpio.h"

void sync_handler();
void irq_handler();
void default_handler();
void enable_interrupt();
void disable_interrupt();
