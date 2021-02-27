#include "shell.h"
#include "cfg.h"
#include "uart.h"
#include <stdbool.h>

char cmd[MX_CMD_BFRSIZE + 1] = {0};
int cmdLen = 0;

void _cmdPush(char c) {
  if (cmdLen < MX_CMD_BFRSIZE) {
    cmd[cmdLen++] = c;
  }
}

void _cmdPop() {
  if (cmdLen > 0) {
    cmd[--cmdLen] = 0;
  }
}

void shellInputLine() {
  enum InputChar c;
  _Bool flagExit = false;
  uart_puts("\r>");
  cmdLen = 0;
  cmd[0] = 0;

  while (!flagExit) {
    flagExit = false;
    switch ((c = uart_getc())) {
    case PRINTABLE_START ... PRINTABLE_END:
      _cmdPush(c);
      uart_send(c);
      break;
    case BackSpace:
    case Delete:
      _cmdPop();
      uart_puts("\b \b");
      break;

    case CarrageReturn:
    case LineFeed:
      flagExit = true;
      cmd[cmdLen] = 0;
      uart_puts("\r\n");
      if (CFG_LOG_ENABLE) {
        uart_puts("GET:'");
        uart_puts(cmd);
        uart_puts("'");
        uart_puts("\r\n");
      }
      break;
    default:
      uart_puts("<?>");
    }
  }
}

// Process command resides in buffer
void shellProcessCommand() {}