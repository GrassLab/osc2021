#include "shell.h"
#include "bool.h"
#include "cfg.h"
#include "string.h"
#include "uart.h"
#include <stdint.h>

void cmdHelp();
void cmdLoadKernel();

typedef struct {
  char *name;
  char *help;
  void (*func)(void);
} Cmd;

char buffer[MX_CMD_BFRSIZE + 1] = {0};
int curInputSize = 0;

Cmd cmdList[] = {
    {.name = "help", .help = "Show avalible commands", .func = cmdHelp},
    {.name = "load",
     .help = "Load kernel image via uart",
     .func = cmdLoadKernel},
};

void cmdHelp() {
  uart_println("available commands:");
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  for (Cmd *c = cmdList; c != end; c++) {
    uart_println("  %s  \t%s", c->name, c->help);
  }
}

int parse_header_field() {
#define HEADER_SIZE 32
  // send digit in plain text to indicate size;
  char data[HEADER_SIZE + 1] = {0};
  for (int i = HEADER_SIZE - 1; i >= 0; i--) {
    data[i] = (char)uart_getu8();
  }
  data[HEADER_SIZE] = 0;

  // parse to int;
  int n = 0;
  for (int i = 0, base = 1; i < HEADER_SIZE; i++, base *= 10) {
    n += (data[i] - '0') * base;
  }
  return n;
}

void cmdLoadKernel() {
  uart_println("load kernel");

  int file_size = parse_header_field();
  uart_println("[header] filesize: %d", file_size);

  int check_sum = parse_header_field();
  uart_println("[header] check_sum: %d", check_sum);

  const int MAGIC_MOD = 856039; // my student id 😎

  // accept data
  int local_check_sum = 0;
  int data = 0;
  for (int i = 0; i < file_size; i++) {
    data = uart_getu8();
    local_check_sum += (int)data;
    local_check_sum %= MAGIC_MOD;
    if (i < 5) {
      uart_println("  recv byte %d: %d, checksum: %d", i, data,
                   local_check_sum);
    }
    if (i == 5) {
      uart_println("  ... omitted");
    }
    if (i > file_size - 5) {
      uart_println("  recv byte %d: %d, checksum: %d", i, data,
                   local_check_sum);
    }
  }
  if (local_check_sum != check_sum) {
    uart_println("Checksum not match, file crashed. QQ");
  }

  uart_println("transmission finished");
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

void shellPrintPrompt() { uart_puts(" >"); };

void shellInputLine() {
  enum KeyboardInput c;
  bool flagExit = false;
  _bfrClear();

  while (!flagExit) {
    flagExit = false;
    switch ((c = uart_getc())) {
    case KI_PRINTABLE_START ... KI_PRINTABLE_END:
      _bfrPush(c);
      uart_send(c);
      break;
    case KI_BackSpace:
    case KI_Delete:
      if (!_bfrPop()) {
        uart_puts("\b \b");
      };
      break;

    case KI_CarrageReturn:
    case KI_LineFeed:
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
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  for (Cmd *c = cmdList; c != end; c++) {
    if (!strcmp(c->name, buffer)) {
      c->func();
    }
  }
}