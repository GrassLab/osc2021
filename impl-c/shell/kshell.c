#include "shell/buffer.h"
#include "shell/cmd.h"
#include "shell/shell.h"

#include "bool.h"
#include "cfg.h"
#include "cpio.h"
#include "string.h"
#include "uart.h"

#include <stdint.h>

typedef enum AnsiEscType {
  Unknown,
  CursorForward,
  CursorBackward,
} AnsiEscType;

enum KeyboardInput {
  KI_BackSpace = '\b',             // 8
  KI_LineFeed = '\n',              // 10
  KI_CarrageReturn = '\r',         // 13
  KI_Esc = '\e',                   // 27
  KI_ANSI_ESCAPE_SEQ_START = '\e', // 27

  KI_PRINTABLE_START = 32,
  KI_PRINTABLE_END = 126,
  KI_Delete = 127,
};

struct InputBuffer bfr;

char bfr_data[MX_CMD_BFRSIZE + 1] = {0};

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

void shellInit() { InputBuffer_init(&bfr, bfr_data, MX_CMD_BFRSIZE); }
void shellInputLine() {
  enum KeyboardInput c;
  AnsiEscType termCtrl;
  bool flagExit = false;
  bfr.clear(&bfr);

  while (!flagExit) {
    flagExit = false;
    _shellUpdatePrompt();
    switch ((c = uart_getc())) {
    case KI_ANSI_ESCAPE_SEQ_START:
      termCtrl = decode_escape_sequence();
      switch (termCtrl) {
      case CursorForward:
        bfr.cursor_mov_right(&bfr);
        break;
      case CursorBackward:
        bfr.cursor_mov_left(&bfr);
        break;
      case Unknown:
        break;
      }
      break;
    case KI_PRINTABLE_START ... KI_PRINTABLE_END:
      bfr.push(&bfr, c);
      uart_send(c);
      break;
    case KI_BackSpace:
    case KI_Delete:
      bfr.pop(&bfr);
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
  Cmd *cmd = getCmd(bfr_data);
  if (cmd != NULL) {
    cmd->func();
  }
  _tryFetchFile();
}