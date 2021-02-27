#include "shell.h"
#include "cfg.h"
#include "string.h"
#include "uart.h"
#include <stdbool.h>

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)0x3F10001c)
#define PM_WDOG ((volatile unsigned int *)0x3F100024)

void cmdHello();
void cmdHelp();
void cmdReboot();

typedef struct {
  char *name;
  char *help;
  void (*func)(void);
} Cmd;

char buffer[MX_CMD_BFRSIZE + 1] = {0};
int curInputSize = 0;

Cmd cmdList[] = {
    {.name = "hello", .help = " Greeting", .func = cmdHello},
    {.name = "help", .help = " Show avalible commands", .func = cmdHelp},
    {.name = "reboot", .help = " Reboot device", .func = cmdReboot},
};

void cmdHello() { uart_puts("Hello 😎👋!!\n"); }
void cmdHelp() {
  int numCmds = sizeof(cmdList) / sizeof(Cmd);
  uart_puts("available commands:\n");
  Cmd *c = cmdList;
  for (int i = 0; i < numCmds; i++, c++) {
    uart_puts("\t");
    uart_puts(c->name);
    uart_puts("\t");
    uart_puts(c->help);
    uart_puts("\n");
  }
}
void cmdReboot() {
  uart_puts("reboot\n");
  *PM_RSTC = PM_PASSWORD | 0x20;
  *PM_WDOG = PM_PASSWORD | 100; // reboot after 100 watchdog ticks
}

void _bfrPush(char c) {
  if (curInputSize < MX_CMD_BFRSIZE) {
    buffer[curInputSize++] = c;
  }
}

int _bfrPop() {
  if (curInputSize > 0) {
    buffer[--curInputSize] = 0;
    return 0;
  }
  return -1;
}

void _bfrClear() {
  curInputSize = 0;
  buffer[0] = 0;
}

void shellInputLine() {
  enum InputChar c;
  _Bool flagExit = false;
  _bfrClear();

  while (!flagExit) {
    flagExit = false;
    switch ((c = uart_getc())) {
    case PRINTABLE_START ... PRINTABLE_END:
      _bfrPush(c);
      uart_send(c);
      break;
    case BackSpace:
    case Delete:
      if (!_bfrPop()) {
        uart_puts("\b \b");
      };
      break;

    case CarrageReturn:
    case LineFeed:
      flagExit = true;
      buffer[curInputSize] = 0;
      uart_puts("\r\n");
      if (CFG_LOG_ENABLE) {
        uart_puts("GET:'");
        uart_puts(buffer);
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
void shellProcessCommand() {
  int numCmds = sizeof(cmdList) / sizeof(Cmd);
  Cmd *c = cmdList;
  for (int i = 0; i < numCmds; i++, c++) {
    if (!strcmp(c->name, buffer)) {
      c->func();
    }
  }
}