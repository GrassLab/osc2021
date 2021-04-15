#include "shell/buffer.h"
#include "shell/shell.h"

#include "bool.h"
#include "cfg.h"
#include "cpio.h"
#include "string.h"
#include "timer.h"
#include "uart.h"

#include <stdint.h>

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)0x3F10001c)
#define PM_WDOG ((volatile unsigned int *)0x3F100024)

void cmdHello();
void cmdLs();
void cmdHelp();
void cmdLoadUser();
void cmdReboot();

typedef struct {
  char *name;
  char *help;
  void (*func)(void);
} Cmd;

struct InputBuffer bfr;

char bfr_data[MX_CMD_BFRSIZE + 1] = {0};

Cmd cmdList[] = {
    {.name = "hello", .help = "Greeting", .func = cmdHello},
    {.name = "ls", .help = "list files", .func = cmdLs},
    {.name = "load_user",
     .help = "load and run user program",
     .func = cmdLoadUser},
    {.name = "help", .help = "Show avalible commands", .func = cmdHelp},
    {.name = "reboot", .help = "Reboot device", .func = cmdReboot},
};

void cmdHello() { uart_println("Hello!!"); }
void cmdHelp() {
  uart_println("available commands:");
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  const int minIndent = 8;
  for (Cmd *c = cmdList; c != end; c++) {
    uart_printf("  %s", c->name);
    for (int i = minIndent - strlen(c->name); i > 0; i--) {
      uart_puts(" ");
    }
    uart_println("%s", c->help);
  }
}
void cmdLs() { cpioLs((void *)RAMFS_ADDR); }
void cmdReboot() {
  uart_println("reboot");
  *PM_RSTC = PM_PASSWORD | 0x20;
  *PM_WDOG = PM_PASSWORD | 100; // reboot after 100 watchdog ticks
}

void cmdLoadUser() {
  uart_println("load user program");
  unsigned long size;
  unsigned char *load_addr = (unsigned char *)0x20000000;
  uint8_t *file =
      (uint8_t *)cpioGetFile((void *)RAMFS_ADDR, "./user_program", &size);
  if (file == NULL) {
    uart_println("Cannot found `user_program.o` under rootfs");
    return;
  }
  if (CFG_LOG_ENABLE) {
    uart_println("  [fetchFile] file addr:%x , size:%d", file, size);
  }
  for (unsigned long i = 0; i < size; i++) {
    load_addr[i] = file[i];
  }
  uart_println("start user app");

  // change exception level
  // asm volatile("mov x0, 0x3c0  \n"); // disable timer interrupt, enable svn
  asm volatile("mov x0, 0x340  \n"); // enable core timer interrupt
  asm volatile("msr spsr_el1, x0  \n");
  asm volatile("msr elr_el1, %0   \n" ::"r"(load_addr));
  asm volatile("msr sp_el0, %0    \n" ::"r"(load_addr));

  // enable the core timer’s interrupt in el0
  timer_el0_enable();
  timer_el0_set_timeout();

  // unmask timer interrupt
  asm volatile("mov x0, 2             \n");
  asm volatile("ldr x1, =0x40000040   \n");
  asm volatile("str w0, [x1]          \n");

  asm volatile("eret              \n");
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
  uart_puts(bfr_data);

  // User might delete 1 character, here we paint a blank space to "delete it"
  // on the screen
  uart_puts(" ");

  // Restore cursor on the screen
  uart_puts("\r\e[");
  uart_puts(itoa(bfr.write_head + 1, 10));
  uart_puts("C");
}

void shellPrintPrompt() { uart_puts("\r>"); }

void shellInit() { bfr_init(&bfr, bfr_data, MX_CMD_BFRSIZE); }
void shellInputLine() {
  enum KeyboardInput c;
  AnsiEscType termCtrl;
  bool flagExit = false;
  bfr_clear(&bfr);

  while (!flagExit) {
    flagExit = false;
    _shellUpdatePrompt();
    switch ((c = uart_getc())) {
    case KI_ANSI_ESCAPE_SEQ_START:
      termCtrl = decode_escape_sequence();
      switch (termCtrl) {
      case CursorForward:
        bfr_cursor_mov_right(&bfr);
        break;
      case CursorBackward:
        bfr_cursor_mov_left(&bfr);
        break;
      case Unknown:
        break;
      }
      break;
    case KI_PRINTABLE_START ... KI_PRINTABLE_END:
      bfr_push(&bfr, c);
      uart_send(c);
      break;
    case KI_BackSpace:
    case KI_Delete:
      bfr_pop(&bfr);
      break;
    case KI_CarrageReturn:
    case KI_LineFeed:
      flagExit = true;
      uart_puts("\r\n");
      if (CFG_LOG_ENABLE) {
        uart_println("buffer: '%s'", bfr_data);
      }
      break;
    default:
        // ignore other input
        ;
    }
  }
}

int _tryFetchFile() {
  unsigned long size;
  uint8_t *file = (uint8_t *)cpioGetFile((void *)RAMFS_ADDR, bfr_data, &size);
  if (file != NULL) {
    if (CFG_LOG_ENABLE) {
      uart_println("  [fetchFile] file addr:%x , size:%d", file, size);
    }
    for (unsigned long i = 0; i < size; i++) {
      if (file[i] == '\n' && i > 0 && file[i - 1] != '\r') {
        uart_println("");
      } else {
        uart_send(file[i]);
      }
    }
    uart_println("");
    return 0;
  }
  return 1;
}

// Process command resides in buffer
void shellProcessCommand() {
  Cmd *end = cmdList + sizeof(cmdList) / sizeof(Cmd);
  for (Cmd *c = cmdList; c != end; c++) {
    if (!strcmp(c->name, bfr_data)) {
      c->func();
      return;
    }
  }
  _tryFetchFile();
}