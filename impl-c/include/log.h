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
#define log_println(fmt, ...)                                                  \
  do {                                                                         \
    if (_DO_LOG) {                                                             \
      uart_println("%s" fmt "%s", "\033[90;m", ##__VA_ARGS__, "\033[0m");      \
    }                                                                          \
  } while (0)

#define log_printf(fmt, ...)                                                   \
  do {                                                                         \
    if (_DO_LOG) {                                                             \
      uart_printf("%s" fmt "%s", "\033[90;m", ##__VA_ARGS__, "\033[0m");       \
    }                                                                          \
  } while (0)
