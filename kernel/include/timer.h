#pragma once

#define CORE0_TIMER_IRQ_CTRL 0x40000040

void core_timer_enable();
void core_timer_handler();
