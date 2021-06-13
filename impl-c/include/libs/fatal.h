#pragma once

#include "uart.h"

#define FATAL(msg)                                                             \
  uart_println(msg);                                                           \
  while (1) {                                                                  \
    ;                                                                          \
  }
