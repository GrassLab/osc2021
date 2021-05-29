/**
 * Log utility func
 * define your _DO_LOG as a const int in your c file to control the actual
 * behaviour
 *
 * For Example:
 * #ifdef CFG_LOG_MEM_BUDDY
 *  static const int _DO_LOG = 1;
 * #else
 *  static const int _DO_LOG = 0;
 * #endif
 */
#include "uart.h"
#define LOG_DIM_START ("\033[90;m")
#define LOG_DIM_END ("\033[0m")

#define log_println(fmt, ...)                                                  \
  do {                                                                         \
    if (_DO_LOG) {                                                             \
      uart_println("%s" fmt "%s", LOG_DIM_START, ##__VA_ARGS__, LOG_DIM_END);  \
    }                                                                          \
  } while (0)

#define log_printf(fmt, ...)                                                   \
  do {                                                                         \
    if (_DO_LOG) {                                                             \
      uart_printf("%s" fmt "%s", LOG_DIM_START, ##__VA_ARGS__, LOG_DIM_END);   \
    }                                                                          \
  } while (0)
