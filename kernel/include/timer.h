#pragma once

#include <stdint.h>

#include "mmu.h"

#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int *)(KVA + 0x40000040))

typedef struct timeout_event {
  uint32_t register_time;
  uint32_t duration;
  void (*callback)(char *);
  char args[20];
  struct timeout_event *prev, *next;
} timeout_event;

timeout_event *timeout_queue_head, *timeout_queue_tail;

void timeout_event_init();
void core_timer_enable();
void core_timer_disable();
void core_timer_handler_lowerEL_64();
void core_timer_handler_currentEL_ELx();
void add_timer(void (*callback)(char *), char *args, uint32_t duration);
uint64_t get_current_time();
void set_expired_time(uint32_t duration);
void timer_callback(char *msg);
