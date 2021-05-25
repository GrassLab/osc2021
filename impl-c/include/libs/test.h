#pragma once

#include "bool.h"
#include "uart.h"

#define TEST_FAILED(name, description)                                         \
  { uart_println("\033[0;31m[TEST][ERR] %s: %s\033[0m", name, description); }

#define TEST_SUCCESS(name, description)                                        \
  { uart_println("\033[0;32m[TEST][OK] %s: %s\033[0m", name, description); }

#define TEST1(test, name, desc)                                                \
  {                                                                            \
    if (test() == true) {                                                      \
      TEST_SUCCESS(name, desc);                                                \
    } else {                                                                   \
      TEST_FAILED(name, desc);                                                 \
    }                                                                          \
  }

static inline void unittest(int (*f)(), char *s, char *de) { TEST1(f, s, de); }

// Return false once the condition does not meet
#define assert(cond)                                                           \
  {                                                                            \
    if (!(cond)) {                                                             \
      return false;                                                            \
    }                                                                          \
  }

void run_tests();

void test_suite_startup_alloc();