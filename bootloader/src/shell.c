#include "shell.h"
#include "bool.h"
#include "cfg.h"
#include "string.h"
#include "uart.h"

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
int bfrWriteHead = 0;
int curInputSize = 0;

Cmd cmdList[] = {
    {.name = "hello", .help = "Greeting", .func = cmdHello},
    {.name = "help", .help = "Show avalible commands", .func = cmdHelp},
    {.name = "reboot", .help = "Reboot device", .func = cmdReboot},
};

void cmdHello() { uart_println("Hello!!"); }
void cmdHelp() {
  uart_println("available commands:");
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  for (Cmd *c = cmdList; c != end; c++) {
    uart_println("  %s  \t%s", c->name, c->help);
  }
}
void cmdReboot() {
  uart_println("reboot");
  *PM_RSTC = PM_PASSWORD | 0x20;
  *PM_WDOG = PM_PASSWORD | 100; // reboot after 100 watchdog ticks
}

void _cursorMoveLeft() {
  if (bfrWriteHead > 0) {
    bfrWriteHead--;
  }
}

void _cursorMoveRight() {
  if (bfrWriteHead < curInputSize) {
    bfrWriteHead++;
  }
}

void _bfrPush(char c) {
  if (curInputSize >= MX_CMD_BFRSIZE)
    // buffer is full
    return;

  if (bfrWriteHead <= curInputSize) {
    // insert in middle: right shift buffer first
    for (int i = curInputSize; i > bfrWriteHead; i--) {
      buffer[i] = buffer[i - 1];
    }
    buffer[bfrWriteHead++] = c;
    curInputSize++;
    buffer[curInputSize] = 0;
  }
}

void _bfrPop() {
  if (bfrWriteHead > 0) {
    bfrWriteHead--;
    // left shift the whole buffer
    for (int i = bfrWriteHead; i < curInputSize; i++) {
      buffer[i] = buffer[i + 1];
    }
    buffer[curInputSize--] = 0;
  }
}

void _bfrClear() {
  curInputSize = 0;
  bfrWriteHead = 0;
  buffer[0] = 0;
}

AnsiEscType decode_escape_sequence() {
  char c = uart_getc();
  if (c == '[') {
    // ANSI CSI
    switch (c = uart_getc()) {
    case 'C':
      return CursorForward;
    case 'D':
      return CursorBackward;
    default:
      return Unknown;
    }
  }
  return Unknown;
}

void _shellUpdatePrompt() {
  // Must be called after every keystroke user input
  // Assumption: There're at most 1 character change inside the buffer

  // Rebuild buffer
  shellPrintPrompt();
  uart_puts(buffer);

  // User might delete 1 character, here we paint a blank space to "delete it"
  // on the screen
  uart_puts(" ");

  // Restore cursor on the screen
  uart_puts("\r\e[");
  uart_puts(itoa(bfrWriteHead + 1, 10));
  uart_puts("C");
}

void shellPrintPrompt() { uart_puts("\r>"); }

void shellInputLine() {
  enum KeyboardInput c;
  AnsiEscType termCtrl;
  bool flagExit = false;
  _bfrClear();

  while (!flagExit) {
    flagExit = false;
    _shellUpdatePrompt();
    switch ((c = uart_getc())) {
    case KI_ANSI_ESCAPE_SEQ_START:
      termCtrl = decode_escape_sequence();
      switch (termCtrl) {
      case CursorForward:
        _cursorMoveRight();
        break;
      case CursorBackward:
        _cursorMoveLeft();
        break;
      case Unknown:
        break;
      }
      break;
    case KI_PRINTABLE_START ... KI_PRINTABLE_END:
      _bfrPush(c);
      uart_send(c);
      break;
    case KI_BackSpace:
    case KI_Delete:
      _bfrPop();
      break;
    case KI_CarrageReturn:
    case KI_LineFeed:
      flagExit = true;
      buffer[curInputSize] = 0;
      uart_puts("\r\n");
      if (CFG_LOG_ENABLE) {
        uart_println("GET:'%s'", buffer);
      }
      break;
    default:
        // ignore other input
        ;
    }
  }
}

// Process command resides in buffer
void shellProcessCommand() {
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  for (Cmd *c = cmdList; c != end; c++) {
    if (!strcmp(c->name, buffer)) {
      c->func();
    }
  }
}